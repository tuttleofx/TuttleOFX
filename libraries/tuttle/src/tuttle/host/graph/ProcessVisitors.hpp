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

		TUTTLE_LOG_TRACE( "[Connect Clips] " << edge );
		TUTTLE_LOG_TRACE( "[Connect Clips] " << vertexOutput << " -> " << vertexInput );
		//TUTTLE_LOG_VAR( TUTTLE_TRACE, edge.getInAttrName() );
		
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
class Setup1 : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	Setup1( TGraph& graph )
		: _graph( graph )
	{}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TUTTLE_LOG_TRACE( "[Setup 1] finish vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode().setup1();
	}

private:
	TGraph& _graph;
};

template<class TGraph>
class Setup2 : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	Setup2( TGraph& graph )
		: _graph( graph )
	{}

	template<class VertexDescriptor, class Graph>
	void discover_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TUTTLE_LOG_TRACE( "[Setup 2] discover vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode().setup2_reverse();
	}

private:
	TGraph& _graph;
};


template<class TGraph>
class Setup3 : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;

	Setup3( TGraph& graph )
		: _graph( graph )
	{}
	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TUTTLE_LOG_TRACE( "[Setup 3] finish vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode().setup3();
	}

private:
	TGraph& _graph;
};

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
	void finish_vertex( VertexDescriptor vd, Graph& g )
	{
		Vertex& vertex = _graph.instance( vd );

		TUTTLE_LOG_TRACE( "[Time Domain] finish vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessData()._timeDomain = vertex.getProcessNode().computeTimeDomain();
		TUTTLE_LOG_TRACE( "[Time Domain] min: " << vertex.getProcessData()._timeDomain.min << ", max: " << vertex.getProcessData()._timeDomain.max );
	}

private:
	TGraph& _graph;
};

template<class TGraph>
class ComputeHashAtTime : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename TGraph::Edge Edge;
	typedef typename TGraph::vertex_descriptor vertex_descriptor;
	typedef typename TGraph::edge_descriptor edge_descriptor;
	typedef typename TGraph::Vertex::Key VertexKey;

	ComputeHashAtTime( TGraph& graph, NodeHashContainer& outNodesHash, const OfxTime time )
		: _graph( graph )
		, _outNodesHash( outNodesHash )
		, _time( time )
	{
		//TUTTLE_LOG_TRACE( "[ComputeHashAtTime] constructor" );
	}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor vd, Graph& g )
	{
		Vertex& vertex = _graph.instance( vd );
		TUTTLE_LOG_TRACE( "[Compute Hash At Time] finish vertex " << vertex );

		if( vertex.isFake() )
			return;

		const std::size_t localHash = vertex.getProcessNode().getLocalHashAtTime(_time);

		typedef std::map<VertexKey, std::size_t> InputsHash;
		InputsHash inputsGlobalHash;
		BOOST_FOREACH( const edge_descriptor& ed, _graph.getOutEdges( vd ) )
		{
			const Edge& edge = _graph.instance( ed );
			vertex_descriptor inputVertexDesc = _graph.target( ed );
			Vertex& inputVertex = _graph.instance( inputVertexDesc );
			
			const std::size_t inputGlobalHash = _outNodesHash.getHash(inputVertex.getKey());
			// Key is: (clipName, time)
			VertexKey k( edge.getInAttrName(), edge.getOutTime() );
			inputsGlobalHash[k] = inputGlobalHash;
		}
		// inputGlobalHashes is put into a map to be ordered by clip name
		// the clipName is unique for each time used
		std::size_t seed = localHash;
		BOOST_FOREACH( const typename InputsHash::value_type& inputGlobalHash, inputsGlobalHash )
		{
			//TUTTLE_LOG_VAR2( TUTTLE_TRACE, inputGlobalHash.first, inputGlobalHash.second );
			boost::hash_combine( seed, inputGlobalHash.first.getName() ); // name of the input clip connected
			boost::hash_combine( seed, inputGlobalHash.second );
		}
		_outNodesHash.addHash( vertex.getKey(), seed );
		//TUTTLE_LOG_VAR( TUTTLE_TRACE, localHash );
		//TUTTLE_LOG_VAR2( TUTTLE_TRACE, vertex.getKey(), seed );
	}

private:
	TGraph& _graph;
	NodeHashContainer& _outNodesHash;
	OfxTime _time;
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
	void discover_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );
	    


		TUTTLE_LOG_TRACE( "[Deploy Time] " << vertex );
		if( vertex.isFake() )
		{
			BOOST_FOREACH( const edge_descriptor& ed, _graph.getOutEdges( v ) )
			{
				Edge& e = _graph.instance( ed );
				e._timesNeeded[_time].insert( _time );
				//TUTTLE_LOG_TRACE( "--- insert edge: " << _time );
			}
			vertex._data._times.insert( _time );
			return;
		}

		// merge times nedded for all out edges
		BOOST_FOREACH( const edge_descriptor& ed, _graph.getInEdges( v ) )
		{
			const Edge& edge = _graph.instance( ed );
			//TUTTLE_LOG_TRACE( "-- outEdge: " << edge );
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
			TUTTLE_LOG_TRACE( "[Deploy Time] time: " << boost::lexical_cast< std::string >(t) );
			INode::ClipTimesSetMap mapInputsTimes = vertex.getProcessNode().getTimesNeeded( t );
//			BOOST_FOREACH( const INode::InputsTimeMap::value_type& v, mapInputsTimes )
//			{
//				TUTTLE_LOG_VAR( TUTTLE_TRACE, v.first );
//			}
			BOOST_FOREACH( const edge_descriptor& ed, _graph.getOutEdges( v ) )
			{
				Edge& edge = _graph.instance( ed );
//				TUTTLE_LOG_INFO( "-- inEdge "<<t<<": " << edge );
//				const Vertex& input = _graph.targetInstance( ed );
//				TUTTLE_LOG_VAR( TUTTLE_TRACE, input.getName() );
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

		TUTTLE_LOG_TRACE( "[Remove identity nodes] finish vertex " << vertex );
		if( vertex.isFake() )
			return;

		std::string inputClip;
		OfxTime atTime;
		if( vertex.getProcessNode().isIdentity( vertex.getProcessDataAtTime(), inputClip, atTime ) )
		{
			try
			{
				IdentityNodeConnection<TGraph> reconnect;
				reconnect._identityVertex = vertex.getKey();
				reconnect._input._inputClip = inputClip;
				
				if( ! inputClip.empty() )
				{
					// The inputClip should not be empty in the OpenFX standard,
					// but it's used in TuttleOFX to disable the process of the node
					// and all the graph branch behind.
					
					bool clipFound = false;
					bool clipFoundAtTime = false;
					BOOST_FOREACH( const edge_descriptor& ed, _graph.getOutEdges( vd ) )
					{
						const Edge& e = _graph.instance( ed );
						if( e.getInAttrName() == inputClip )
						{
							clipFound = true;
							if( e.getOutTime() == atTime )
							{
								clipFoundAtTime = true;
								vertex_descriptor in = _graph.target( ed );
								reconnect._input._srcNode = _graph.instance(in).getKey();
							}
						}
					}
					if( ! clipFound )
					{
						std::stringstream existingClips;
						BOOST_FOREACH( const edge_descriptor& ed, _graph.getOutEdges( vd ) )
						{
							const Edge& e = _graph.instance( ed );
							existingClips << e.getInAttrName() << ", ";
						}
						BOOST_THROW_EXCEPTION( exception::Bug()
								<< exception::dev() + "Plugin declares itself as identity, but gives a non existing input clip. "
									"(node: " + vertex.getName() + ", input clip:" + inputClip + ", existing clips: (" + existingClips.str() + "))"
								);
					}
					if( ! clipFoundAtTime )
					{
						std::stringstream framesNeeded;
						BOOST_FOREACH( const edge_descriptor& ed, _graph.getOutEdges( vd ) )
						{
							const Edge& e = _graph.instance( ed );
							if( e.getInAttrName() == inputClip )
							{
								framesNeeded << e.getOutTime() << ", ";
							}
						}
						BOOST_THROW_EXCEPTION( exception::Bug()
								<< exception::dev() + "Plugin declares itself as identity, but gives a time on the input clip different from declared framesNeeded. "
									"(node: " + vertex.getName() + ", input clip:" + inputClip + ", time:" + atTime + ", framesNeeded:(" + framesNeeded.str() + "))"
								);
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
				}
				TUTTLE_LOG_TRACE( "isIdentity => " << vertex.getName() << " - " << inputClip << " at time " << atTime );
				_toRemove.push_back( reconnect );
				TUTTLE_LOG_VAR( TUTTLE_TRACE, _toRemove.size() );
			}
			catch( boost::exception& e )
			{
				e << exception::user() + "Error on the node " + quotes(vertex.getName()) + ". It fails to declare itself as an identity node."
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
//		TUTTLE_LOG_VAR( TUTTLE_TRACE, toRemove.size() );
//		BOOST_FOREACH( const IdentityNodeConnection<TGraph>& connection, toRemove )
//		{
//			TUTTLE_LOG_TRACE( connection._identityVertex );
//			TUTTLE_LOG_TRACE( "IN: "
//				<< connection._identityVertex << "::" << connection._input._inputClip
//				<< " <<-- "
//				<< connection._input._srcNode << "::" kOfxOutputAttributeName );
//			
//			BOOST_FOREACH( const typename IdentityNodeConnection<TGraph>::OutputClipConnection& outputClipConnection, connection._outputs )
//			{
//				TUTTLE_LOG_TRACE( "OUT: "
//					<< connection._identityVertex << "::" kOfxOutputAttributeName
//					<< " -->> "
//					<< outputClipConnection._dstNode << "::" << outputClipConnection._dstNodeClip );
//			}
//		}
//	);
//	TUTTLE_LOG_TRACE( "-- removeIdentityNodes --" );
	typedef typename TGraph::Vertex Vertex;
	typedef typename Vertex::Key VertexKey;
	
	typedef boost::unordered_map<VertexKey, const IdentityNodeConnection<TGraph>*> IdentityMap;
	IdentityMap identityNodes;
	
	BOOST_FOREACH( const IdentityNodeConnection<TGraph>& connection, nodesToRemove )
	{
		identityNodes[connection._identityVertex] = &connection;
	}
	
	bool disabledBranch = false;
	BOOST_FOREACH( const IdentityNodeConnection<TGraph>& connection, nodesToRemove )
	{
		if( connection._input._inputClip.empty() )
		{
			// The input clip is empty, this not in the OpenFX standard.
			// In Tuttle, it means that the branch is disabled.
			disabledBranch = true;
		}
		else
		{
			// Create new connections if the input clip name is not empty
			TUTTLE_LOG_TRACE( boost::lexical_cast<std::string>(connection._identityVertex) );
			TUTTLE_LOG_TRACE( "IN: "
				<< boost::lexical_cast<std::string>(connection._identityVertex) << "::" << boost::lexical_cast<std::string>(connection._input._inputClip)
				<< " <<-- "
				<< boost::lexical_cast<std::string>(connection._input._srcNode) << "::" kOfxOutputAttributeName );
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
				//TUTTLE_LOG_TRACE( "OUT: "
				//	<< connection._identityVertex << "::" kOfxOutputAttributeName
				//	<< " -->> "
				//	<< outputClipConnection._dstNode << "::" << outputClipConnection._dstNodeClip );
				const typename TGraph::VertexKey& out = outputClipConnection._dstNode;
				const std::string inAttr = outputClipConnection._dstNodeClip;

				const typename TGraph::vertex_descriptor descOut = graph.getVertexDescriptor( out );

				const typename TGraph::Edge e( in, out, inAttr );
				graph.addEdge( descOut, descIn, e );
			}
		}
		// Warning: We don't remove the vertex itself to not invalidate vertex_descriptors but only remove edges.
//		graph.removeVertex( graph.getVertexDescriptor(connection._identityVertex) );
		// remove all node connections
		graph.clearVertex( graph.getVertexDescriptor(connection._identityVertex) );
	}
	
	if( disabledBranch )
	{
		// @todo: Remove connections of all vertices not connected to the output.
		// These unconnected branches have been created by identity nodes without source clip.
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

		TUTTLE_LOG_TRACE( "[Preprocess 1] finish vertex " << vertex );
		if( vertex.isFake() )
			return;

		//TUTTLE_LOG_TRACE( vertex.getProcessDataAtTime()._time );
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

		TUTTLE_LOG_TRACE( "[Preprocess 2] discover vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode().preProcess2_reverse( vertex.getProcessDataAtTime() );
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


//		TUTTLE_LOG_TRACE( vertex.getName() );
//		TUTTLE_LOG_TRACE( procOptions );
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
		TUTTLE_LOG_TRACE( "[Process] finish_vertex " << vertex );

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
		
		TUTTLE_LOG_TRACE( "[Process] " << quotes(vertex._name) << " " << vertex._data._time << " took: " << t2 - t1 << " (cumul: " << _cumulativeTime << ")" << vertex );
		
		if( _result && vertex.getProcessDataAtTime()._isFinalNode )
		{
			memory::CACHE_ELEMENT img = _cache.get( vertex._clipName + "." kOfxOutputAttributeName, vertex._data._time );
			if( ! img.get() )
			{
				BOOST_THROW_EXCEPTION( exception::Logic()
					<< exception::user() + "Output buffer not found in memoryCache at the end of the node process."
					<< exception::dev() + vertex._clipName + "." kOfxOutputAttributeName + " at time " + vertex._data._time
					<< exception::nodeName( vertex._name )
					<< exception::time( vertex._data._time ) );
			}
			_result->put( vertex._clipName, vertex._data._time, img );
		}
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

		TUTTLE_LOG( TUTTLE_TRACE,"[Post-process] initialize_vertex " << vertex );
		if( vertex.isFake() )
			return;
	}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TUTTLE_LOG_TRACE( "[Post-process] finish_vertex " << vertex );
		if( vertex.isFake() )
			return;

		vertex.getProcessNode().postProcess( vertex.getProcessDataAtTime() );
	}

private:
	TGraph& _graph;
};


template<class TGraph>
class BeforeRenderCallbackVisitor : public boost::default_dfs_visitor
{
public:
	typedef typename TGraph::Vertex Vertex;
    BeforeRenderCallbackVisitor( TGraph& graph )
        : _graph( graph )
    {}

    template<class VertexDescriptor, class Graph>
    void finish_vertex( VertexDescriptor v, Graph& g )
    {
        Vertex& vertex = _graph.instance( v );
		if( !vertex.isFake() )
		{
            vertex.getProcessNode().beforeRenderCallback( vertex.getProcessNode(), vertex.getProcessDataAtTime() );
        }
    }

private:
	TGraph& _graph;
};


}
}
}
}

#endif

