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

#include <iostream>
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
		typename TGraph::Vertex& vertexSource = graph.sourceInstance( ed );
		typename TGraph::Vertex& vertexDest   = graph.targetInstance( ed );

		TUTTLE_TCOUT( "[connectClips] " << edge );
		TUTTLE_TCOUT( vertexSource << "->" << vertexDest );
		
		if( ! vertexDest.isFake() && ! vertexSource.isFake() )
		{
			INode& sourceNode = vertexSource.getProcessNode();
			INode& targetNode = vertexDest.getProcessNode();
			sourceNode.connect( targetNode, sourceNode.getAttribute( edge.getInAttrName() ) );
		}
	}
}

namespace visitor {

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
	typedef typename TGraph::edge_descriptor edge_descriptor;

	DeployTime( TGraph& graph, const OfxTime time )
		: _graph( graph )
		, _time( time )
	{}

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
			INode::InputsTimeMap mapInputsTimes = vertex.getProcessNode().getTimesNeeded( t );
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
	typedef typename TGraph::Edge Edge;
	typedef typename TGraph::edge_descriptor edge_descriptor;

	RemoveIdentityNodes( TGraph& graph )
		: _graph( graph )
	{}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor vd, Graph& g )
	{
		Vertex& vertex = _graph.instance( vd );

		TUTTLE_TCOUT( "[RemoveIdentityNodes] finish_vertex " << vertex );
		if( vertex.isFake() )
			return;

		std::string clip;
		OfxTime time;
		if( vertex.getProcessNode().isIdentity( vertex.getProcessDataAtTime(), clip, time ) )
		{
			try
			{
				if( clip.size() == 0 )
				{
					BOOST_THROW_EXCEPTION( exception::Logic()
						<< exception::dev() + "Empty clip name. It's seems to be an error in the plugin" );
				}
//				Vertex& replace = _graph.getVertex( VertexKey( clip, time ) );
				VertexKey replaceKey( clip, time );
				BOOST_FOREACH( const edge_descriptor& ed, _graph.getInEdges( vd ) )
				{
					const Edge& e = _graph.instance( ed );
					const Vertex& in = _graph.sourceInstance( ed );
					_graph.connect( replaceKey,
					                in.getKey(),
					                e.getInAttrName() );
				}
				_graph.removeVertex( vd );
			}
			catch( boost::exception& e )
			{
				e << exception::user() + "A node is declared identity without given a valid input clip ("+quotes(clip)+", "+time+ ")."
				  << exception::nodeName( vertex.getName() )
				  << exception::time( vertex.getProcessDataAtTime()._time );
				throw;
			}
		}
	}

private:
	TGraph& _graph;
};

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
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );

		TUTTLE_TCOUT( "[PREPROCESS 2] finish_vertex " << vertex );
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
			vertex.getProcessNode().preProcess_infos( procOptions._time, procOptions._localInfos );
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

	Process( TGraph& graph, memory::IMemoryCache& cache, memory::IMemoryCache& result )
		: _graph( graph )
		, _cache( cache )
		, _result( result )
	{
	}

	template<class VertexDescriptor, class Graph>
	void finish_vertex( VertexDescriptor v, Graph& g )
	{
		Vertex& vertex = _graph.instance( v );
//		TUTTLE_COUT( "[PROCESS] finish_vertex " << vertex );

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
		
//		TUTTLE_COUT( "** Process " << quotes(vertex._name) << " " << vertex._data._time << " took: " << t2 - t1 << " (cumul: " << _cumulativeTime << ")" );
		std::cout << "** Process " << quotes(vertex._name) << " " << vertex._data._time << " took: " << t2 - t1 << " (cumul: " << _cumulativeTime << ")" << std::endl;
		
		if( vertex.getProcessDataAtTime()._finalNode )
		{
			memory::CACHE_ELEMENT img = _cache.get( vertex._name + "." kOfxOutputAttributeName, vertex._data._time );
			if( ! img.get() )
			{
				BOOST_THROW_EXCEPTION( exception::Logic()
					<< exception::user() + "Output buffer not found in memoryCache at the end of the node process."
					<< exception::nodeName( vertex._name )
					<< exception::time( vertex._data._time ) );
			}
			_result.put( vertex._name, vertex._data._time, img );
		}
	}

private:
	TGraph& _graph;
	memory::IMemoryCache& _cache;
	memory::IMemoryCache& _result;
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

