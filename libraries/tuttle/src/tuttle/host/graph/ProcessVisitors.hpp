#ifndef _TUTTLE_HOST_PROCESSVISITORS_HPP_
#define _TUTTLE_HOST_PROCESSVISITORS_HPP_

#include "ProcessVertexData.hpp"

#include <tuttle/host/memory/MemoryCache.hpp>

#include <boost/graph/properties.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/unordered_map.hpp>

#include <iostream>
#include <fstream>
#include <vector>

namespace tuttle {
namespace host {
namespace graph {

template<class TGraph>
inline void connectClips( TGraph& graph )
{
	BOOST_FOREACH( typename TGraph::edge_descriptor ed, graph.getEdges() )
	{
		typename TGraph::Edge& edge           = graph.instance( ed );
		typename TGraph::Vertex& vertexOutput = graph.targetInstance( ed );
		typename TGraph::Vertex& vertexInput  = graph.sourceInstance( ed );

		TUTTLE_TCOUT( "[connectClips] " << edge );
		TUTTLE_TCOUT( vertexOutput << " -> " << vertexInput );
		//TUTTLE_TCOUT_VAR( edge.getInAttrName() );
		
		if( ! vertexOutput.isFake() && ! vertexInput.isFake() )
		{
			INode& outputNode = vertexOutput.getProcessNode();
			INode& inputNode = vertexInput.getProcessNode();
			inputNode.connect( outputNode, inputNode.getAttribute( edge.getInAttrName() ) );
		}
	}
}


namespace visitor {



template<class TGraph>
class TimeDomain : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	TimeDomain( TGraph& graph )
		: _graph( graph )
	{}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TUTTLE_TCOUT( "[TimeDomain] finish_vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode().getTimeDomain( vertex.getProcessData()._timeDomain );
		TUTTLE_TCOUT_VAR2( vertex.getProcessData()._timeDomain.min, vertex.getProcessData()._timeDomain.max );
	}

private:
	TGraph& _graph;
};


/**
 * @brief Create a new version of a graph with nodes deployed over time.
 *
 * Compute kOfxImageEffectActionGetFramesNeeded for each node if kOfxImageEffectPropTemporalClipAccess is true.
 */
template<class TGraph>
class DeployTime : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename TGraph::Edge Edge;
	typedef typename TGraph::vertex_descriptor vertex_descriptor;
	typedef typename TGraph::edge_descriptor edge_descriptor;

	DeployTime( TGraph& graph, const OfxTime time )
		: _graph( graph )
		, _time( time )
	{
		// clear all time informations before to fill with new values
		BOOST_FOREACH( const vertex_descriptor vd, graph.getVertices() )
		{
			Vertex& v = graph.instance( vd );
			v.clearTimeInfo();
		}
		BOOST_FOREACH( const edge_descriptor ed, graph.getEdges() )
		{
			Edge& e = graph.instance( ed );
			e.clearTimeInfo();
		}
	}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );
		
		TUTTLE_TCOUT( "[DEPLOY TIME] " << vertex );
		if( vertex.isFake() )
		{
			BOOST_FOREACH( const edge_descriptor& ed, _graph.getOutEdges( v ) )
			{
				Edge& e = _graph.instance( ed );
				e._timesNeeded[_time].insert( _time );
//				TUTTLE_TCOUT( "--- insert edge: " << _time );
			}
			vertex._data._times.insert( _time );
			return;
		}

		// merge times nedded for all out edges
		BOOST_FOREACH( const edge_descriptor& ed, _graph.getInEdges( v ) )
		{
			const Edge& edge = _graph.instance( ed );
//			TUTTLE_TCOUT( "-- outEdge: " << edge );
			BOOST_FOREACH( const typename Edge::TimeMap::value_type& timesNeeded, edge._timesNeeded )
			{
				vertex._data._times.insert( timesNeeded.second.begin(), timesNeeded.second.end() );
//				std::cout << "--- insert vertex: ";
//				std::copy( (*timesNeeded).second.begin(),
//				           (*timesNeeded).second.end(),
//						   std::ostream_iterator<OfxTime>(std::cout, ",") );
//				std::cout << std::endl;
			}
		}
		
		// Set all times needed on each input edges
		BOOST_FOREACH( const OfxTime t, vertex._data._times )
		{
			TUTTLE_TCOUT( "-  time: "<< t );
			INode::ClipTimesSetMap mapInputsTimes = vertex.getProcessNode().getTimesNeeded( t );
//			BOOST_FOREACH( const INode::InputsTimeMap::value_type& v, mapInputsTimes )
//			{
//				TUTTLE_TCOUT_VAR( v.first );
//			}
			BOOST_FOREACH( const edge_descriptor& ed, _graph.getOutEdges( v ) )
			{
				Edge& edge = _graph.instance( ed );
//				TUTTLE_TCOUT( "-- inEdge "<<t<<": " << edge );
//				const Vertex& input = _graph.targetInstance( ed );
//				TUTTLE_TCOUT_VAR( input.getName() );
//				std::cout << "--- insert edges: ";
//				std::copy( mapInputsTimes[input.getName()+"." kOfxOutputAttributeName].begin(),
//				           mapInputsTimes[input.getName()+"." kOfxOutputAttributeName].end(),
//						   std::ostream_iterator<OfxTime>(std::cout, ",") );
				edge._timesNeeded[t] = mapInputsTimes[edge.getInAttrName()];
			}
		}
	}

private:
	TGraph& _graph;
	OfxTime _time;
};

template<class TGraph>
struct IdentityNodeConnection
{
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename Vertex::Key VertexKey;
	typedef typename TGraph::vertex_descriptor vertex_descriptor;
	typedef typename TGraph::Edge Edge;
	typedef typename TGraph::edge_descriptor edge_descriptor;
	
	VertexKey _identityVertex; ///< the identity node to remove
	struct InputClipConnection
	{
		VertexKey _srcNode;
		std::string _inputClip;
	};
	struct OutputClipConnection
	{
		VertexKey _dstNode;
		std::string _dstNodeClip;
	};
	InputClipConnection _input;
	std::vector< OutputClipConnection > _outputs;
};

/**
 * @warning in progress...
 */
template<class TGraph>
class RemoveIdentityNodes : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename Vertex::Key VertexKey;
	typedef typename TGraph::vertex_descriptor vertex_descriptor;
	typedef typename TGraph::Edge Edge;
	typedef typename TGraph::edge_descriptor edge_descriptor;

public:
	RemoveIdentityNodes( TGraph& graph, std::vector<IdentityNodeConnection<TGraph> >& toRemove )
		: _graph( graph )
		, _toRemove( toRemove )
	{}
	
	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor vd, Graph& g )
	{
		Vertex& vertex = _graph.instance( vd );

		TUTTLE_TCOUT( "[RemoveIdentityNodes] finish_vertex " << vertex );
		if( vertex.isFake() )
			return;

		std::string inputClip;
		OfxTime atTime;
		if( vertex.getProcessNode().isIdentity( vertex.getProcessDataAtTime(), inputClip, atTime ) )
		{
			try
			{
				if( inputClip.size() == 0 )
				{
					BOOST_THROW_EXCEPTION( exception::Logic()
						<< exception::dev() + "There is an error in the plugin: The plugin declares to be identity but don't give the name of the input clip to use." );
				}
				IdentityNodeConnection<TGraph> reconnect;
				reconnect._identityVertex = _graph.instance(vd).getKey();
				reconnect._input._inputClip = inputClip;
				BOOST_FOREACH( const edge_descriptor& ed, _graph.getOutEdges( vd ) )
				{
					const Edge& e = _graph.instance( ed );
					if( ( e.getInAttrName() == inputClip ) &&
					    ( e.getOutTime() == atTime )
					  )
					{
						vertex_descriptor in = _graph.target( ed );
						reconnect._input._srcNode = _graph.instance(in).getKey();
					}
				}
				BOOST_FOREACH( const edge_descriptor& ed, _graph.getInEdges( vd ) )
				{
					const Edge& e = _graph.instance( ed );
					vertex_descriptor out = _graph.source( ed );
					typename IdentityNodeConnection<TGraph>::OutputClipConnection c;
					c._dstNode = _graph.instance(out).getKey();
					c._dstNodeClip = e.getInAttrName();
					reconnect._outputs.push_back(c);
				}
				TUTTLE_TCOUT( "isIdentity => " << vertex.getName() << " - " << inputClip << " at time " << atTime );
				_toRemove.push_back( reconnect );
				TUTTLE_TCOUT_VAR( _toRemove.size() );
			}
			catch( boost::exception& e )
			{
				e << exception::user() + "A node is declared identity without given a valid input clip ("+quotes(inputClip)+", "+atTime+ ")."
				  << exception::nodeName( vertex.getName() )
				  << exception::time( vertex.getProcessDataAtTime()._time );
				throw;
			}
		}
	}

private:
	TGraph& _graph;
	std::vector<IdentityNodeConnection<TGraph> >& _toRemove;
	
};

/**
 * @brief Unconnect identity nodes and re-connect neightbors nodes
 *
 * ////////////////////////////////////////////////////////////////////////////////
 * //        Output at time 5
 * //          |
 * //        Retime time -2 <-- identity node declare to use Merge at time 3
 * //          |
 * //        Merge at time 3 <-- identity node declare to use ReadA at time 3
 * //        /    \
 * //      /        \
 * //    /            \
 * // ReadA time 3   ReadB time 3
 * //
 * ////////////////////////////////////////////////////////////////////////////////
 * // After removing identity nodes:
 * //
 * // Output at time 5
 * //   |
 * // ReadA time 3
 * //
 * // Retime time -2 <-- leave unconnected  
 * // Merge at time 3 <-- leave unconnected  
 * // ReadB time 3 <-- leave unconnected
 * ////////////////////////////////////////////////////////////////////////////////
 */
template<class TGraph>
void removeIdentityNodes( TGraph& graph, const std::vector<IdentityNodeConnection<TGraph> >& nodesToRemove )
{
//	TUTTLE_IF_DEBUG(
//		TUTTLE_COUT_X( 80, "_" );
//		TUTTLE_COUT_VAR( toRemove.size() );
//		BOOST_FOREACH( const IdentityNodeConnection<TGraph>& connection, toRemove )
//		{
//			TUTTLE_COUT( connection._identityVertex );
//			TUTTLE_COUT( "IN: "
//				<< connection._identityVertex << "::" << connection._input._inputClip
//				<< " <<-- "
//				<< connection._input._srcNode << "::" kOfxOutputAttributeName );
//			
//			BOOST_FOREACH( const typename IdentityNodeConnection<TGraph>::OutputClipConnection& outputClipConnection, connection._outputs )
//			{
//				TUTTLE_COUT( "OUT: "
//					<< connection._identityVertex << "::" kOfxOutputAttributeName
//					<< " -->> "
//					<< outputClipConnection._dstNode << "::" << outputClipConnection._dstNodeClip );
//			}
//		}
//		TUTTLE_COUT_X( 80, "_" );
//	);
//	TUTTLE_COUT( "-- removeIdentityNodes --" );
	typedef typename TGraph::Vertex Vertex;
	typedef typename Vertex::Key VertexKey;
	
	typedef boost::unordered_map<VertexKey, const IdentityNodeConnection<TGraph>*> IdentityMap;
	IdentityMap identityNodes;
	
	BOOST_FOREACH( const IdentityNodeConnection<TGraph>& connection, nodesToRemove )
	{
		identityNodes[connection._identityVertex] = &connection;
	}
	
	BOOST_FOREACH( const IdentityNodeConnection<TGraph>& connection, nodesToRemove )
	{
		TUTTLE_COUT( connection._identityVertex );
		TUTTLE_COUT( "IN: "
			<< connection._identityVertex << "::" << connection._input._inputClip
			<< " <<-- "
			<< connection._input._srcNode << "::" kOfxOutputAttributeName );
		const typename TGraph::VertexKey* searchIn = &( connection._input._srcNode );
		{
			// search a non-identity node to replace the connection
			typename IdentityMap::const_iterator it = identityNodes.find( *searchIn );
			typename IdentityMap::const_iterator itEnd = identityNodes.end();
			while( it != itEnd )
			{
				searchIn = &( it->second->_input._srcNode );
				it = identityNodes.find( *searchIn );
			}
		}
		const typename TGraph::VertexKey& in = *searchIn;
		const typename TGraph::vertex_descriptor descIn  = graph.getVertexDescriptor( in );
		
		// replace all input/output connections of the identity node by one edge
		BOOST_FOREACH( const typename IdentityNodeConnection<TGraph>::OutputClipConnection& outputClipConnection, connection._outputs )
		{
			//TUTTLE_COUT( "OUT: "
			//	<< connection._identityVertex << "::" kOfxOutputAttributeName
			//	<< " -->> "
			//	<< outputClipConnection._dstNode << "::" << outputClipConnection._dstNodeClip );
			const typename TGraph::VertexKey& out = outputClipConnection._dstNode;
			const std::string inAttr = outputClipConnection._dstNodeClip;

			const typename TGraph::vertex_descriptor descOut = graph.getVertexDescriptor( out );

			const typename TGraph::Edge e( in, out, inAttr );
			graph.addEdge( descOut, descIn, e );
		}
		// Warning: We don't remove the vertex itself to not invalidate vertex_descriptors but only modify edges.
//		graph.removeVertex( graph.getVertexDescriptor(connection._identityVertex) );
		// remove all node connections
		graph.clearVertex( graph.getVertexDescriptor(connection._identityVertex) );
	}
}

template<class TGraph>
class PreProcess1 : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	PreProcess1( TGraph& graph )
		: _graph( graph )
	{}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TUTTLE_TCOUT( "[PREPROCESS 1] finish_vertex " << vertex );
		if( vertex.isFake() )
			return;

		TUTTLE_TCOUT( vertex.getProcessDataAtTime()._time );
		vertex.getProcessNode().preProcess1( vertex.getProcessDataAtTime() );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
class PreProcess2 : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	PreProcess2( TGraph& graph )
		: _graph( graph )
	{}

	template<class VertexDescriptor, class Graph>
	void discover_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TUTTLE_TCOUT( "[PREPROCESS 2] discover_vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode().preProcess2_reverse( vertex.getProcessDataAtTime() );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
class PreProcess3 : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	PreProcess3( TGraph& graph )
		: _graph( graph )
	{}
	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TUTTLE_TCOUT( "[PREPROCESS 3] finish_vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode().preProcess3( vertex.getProcessDataAtTime() );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
class OptimizeGraph : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename TGraph::Edge Edge;
	typedef typename TGraph::vertex_descriptor vertex_descriptor;
	typedef typename TGraph::edge_descriptor edge_descriptor;
	typedef typename TGraph::in_edge_iterator in_edge_iterator;
	typedef typename TGraph::out_edge_iterator out_edge_iterator;

	OptimizeGraph( TGraph& graph )
		: _graph( graph )
	{
//		TUTTLE_COUT_X( 80, ":" );
	}

	template <class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, const Graph& g )
	{
		using namespace boost;
		using namespace boost::graph;
		Vertex& vertex = _graph.instance( v );

		ProcessVertexAtTimeData& procOptions = vertex.getProcessDataAtTime();
		if( !vertex.isFake() )
		{
			// compute local infos, need to be a real node !
			vertex.getProcessNode().preProcess_infos( vertex.getProcessDataAtTime(), procOptions._time, procOptions._localInfos );
		}

		// compute global infos for inputs

		// direct dependencies (originally the node inputs)
		BOOST_FOREACH( const edge_descriptor& oe, out_edges( v, _graph.getGraph() ) )
		{
//			Edge& outEdge = _graph.instance(*oe);
			Vertex& outVertex = _graph.targetInstance( oe );
			procOptions._inputsInfos += outVertex.getProcessDataAtTime()._localInfos;
			procOptions._globalInfos += outVertex.getProcessDataAtTime()._globalInfos;
		}
		procOptions._globalInfos += procOptions._localInfos;

//		BOOST_FOREACH( const edge_descriptor& ie, in_edges( v, _graph.getGraph() ) )
//		{
//			Edge& e = _graph.instance( ie );
//		}


//		TUTTLE_TCOUT_X( 80, "." );
//		TUTTLE_TCOUT( vertex.getName() );
//		TUTTLE_TCOUT( procOptions );
//		TUTTLE_TCOUT_X( 80, "." );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
class Process : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	Process( TGraph& graph, memory::IMemoryCache& cache )
		: _graph( graph )
		, _cache( cache )
		, _result( NULL )
	{
	}
	
	Process( TGraph& graph, memory::IMemoryCache& cache, memory::IMemoryCache& result )
		: _graph( graph )
		, _cache( cache )
		, _result( &result )
	{
	}
	
	/**
	 * Set a MemoryCache object to accumulate output nodes buffers.
	 */
	void setOutputMemoryCache( memory::IMemoryCache& result )
	{
		_result = &result;
	}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );
		TUTTLE_TCOUT_X( 80, "_" );
		TUTTLE_TCOUT( "[PROCESS] finish_vertex " << vertex );

		// do nothing on the empty output node
		// it's just a link to final nodes
		if( vertex.isFake() )
			return;

		// check if abort ?

		// launch the process
		boost::posix_time::ptime t1(boost::posix_time::microsec_clock::local_time());
		vertex.getProcessNode().process( vertex.getProcessDataAtTime() );
		boost::posix_time::ptime t2(boost::posix_time::microsec_clock::local_time());
		_cumulativeTime += t2 - t1;
		
//		TUTTLE_COUT( "\t-> " << quotes(vertex._name) << " " << vertex._data._time << " took: " << t2 - t1 << " (cumul: " << _cumulativeTime << ")" );
#ifndef TUTTLE_PRODUCTION
		std::cout << "\t-> " << quotes(vertex._name) << " " << vertex._data._time << " took: " << t2 - t1 << " (cumul: " << _cumulativeTime << ")" << std::endl;
#else
		//std::cout << " " << quotes(vertex._name) << std::flush;
#endif
		
		if( _result && vertex.getProcessDataAtTime()._isFinalNode )
		{
			memory::CACHE_ELEMENT img = _cache.get( vertex._clipName + "." kOfxOutputAttributeName, vertex._data._time );
			if( ! img.get() )
			{
				BOOST_THROW_EXCEPTION( exception::Logic()
					<< exception::user() + "Output buffer not found in memoryCache at the end of the node process."
					<< exception::nodeName( vertex._name )
					<< exception::time( vertex._data._time ) );
			}
			_result->put( vertex._clipName, vertex._data._time, img );
		}
		
		TUTTLE_TCOUT_X( 40, "_-" );
	}

private:
	TGraph& _graph;
	memory::IMemoryCache& _cache;
	memory::IMemoryCache* _result;
	boost::posix_time::time_duration _cumulativeTime;
};

template<class TGraph>
class PostProcess : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	PostProcess( TGraph& graph )
		: _graph( graph )
	{}

	template<class VertexDescriptor, class Graph>
	void initialize_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

//		TUTTLE_TCOUT( "[POSTPROCESS] initialize_vertex " << vertex );
		if( vertex.isFake() )
			return;
	}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

//		TUTTLE_TCOUT( "[POSTPROCESS] finish_vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode().postProcess( vertex.getProcessDataAtTime() );
	}

private:
	TGraph& _graph;
};

}
}
}
}

#endif

