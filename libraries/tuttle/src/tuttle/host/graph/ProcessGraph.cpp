#include "ProcessGraph.hpp"
#include "ProcessVisitors.hpp"
#include <tuttle/host/graph/GraphExporter.hpp>

#include <boost/foreach.hpp>

namespace tuttle {
namespace host {
namespace graph {

const std::string ProcessGraph::_outputId( "TUTTLE_FAKE_OUTPUT" );

ProcessGraph::ProcessGraph( Graph& graph, const std::list<std::string>& outputNodes )
	: _instanceCount( graph.getInstanceCount() )
{
	_graph.copyTransposed( graph.getGraph() );

	Vertex outputVertex( _outputId );
	_graph.addVertex( outputVertex );

	BOOST_FOREACH( const std::string & s, outputNodes )
	{
		_graph.connect( _outputId, s, "Output" );
		COUT_DEBUG( "MY OUTPUT: " << s );
	}
	relink();
}

ProcessGraph::~ProcessGraph()
{}

/**
 * @brief After copying Vertices, we need to duplicate Nodes and relink Vertices with new Nodes.
 */
void ProcessGraph::relink()
{
	_graph.removeUnconnectedVertices( _graph.getVertexDescriptor( _outputId ) );

	BOOST_FOREACH( InternalGraphImpl::vertex_descriptor vd, _graph.getVertices() )
	{
		Vertex& v = _graph.instance( vd );

		// fake node has no ProcessNode
		if( !v.isFake() )
		{
#ifdef PROCESSGRAPH_USE_LINK
			tuttle::host::INode& origNode = v.getProcessNode(); // pointer of the copied graph, we don't owns it !
#else
			const tuttle::host::INode& origNode = v.getProcessNode(); // pointer of the copied graph, we don't owns it !
#endif
			std::string key( origNode.getName() );
			NodeMap::iterator it = _nodes.find( key );
			tuttle::host::INode* newNode;
			if( it != _nodes.end() )
			{
				newNode = it->second;
			}
			else
			{
#ifdef PROCESSGRAPH_USE_LINK
				newNode = &origNode;
				_nodes[key] = dynamic_cast<Node*>( newNode ); // link to the original node
#else
				newNode = origNode.clone();
				/// @todo tuttle: no dynamic_cast here, _nodes must use tuttle::host::Node
				_nodes.insert( key, dynamic_cast<Node*>( newNode ) ); // owns the new pointer
#endif
			}
			// our vertices have a link to our Nodes
			v.setProcessNode( newNode );
		}
	}
}

/*
   void removeVertexAndReconnectTo( const VertexDescriptor& v, const VertexDescriptor& other )
   {
    InternalGraph::out_edge_iterator oe, oeEnd;
    tie(oe, oeEnd) = out_edges(v, g);
    // InternalGraph::in_edge_iterator ie, ieEnd;
    // tie(ie, ieEnd) = in_edges(v, g);

    for( ; oe != oeEnd; ++oe )
        source( oe )

    _graph.removeVertex( v );
   }
 */

/*
   // May be interesting for process function.
   typedef std::vector< Vertex > container;
   container c;
   topological_sort( G, std::back_inserter(c) );

   cout << "A topological ordering: ";
   for( container::reverse_iterator ii=c.rbegin(); ii!=c.rend(); ++ii )
   cout << index(*ii) << " ";
   cout << endl;
*/
/*
template<class TGraph>
class SortEdgeByMemorySize
{
public:
	typedef typename TGraph::GraphContainer GraphContainer;
	typedef typename TGraph::Vertex Vertex;
	typedef typename TGraph::Edge Edge;
	typedef typename TGraph::edge_descriptor edge_descriptor;

	SortEdgeByMemorySize( const TGraph& graph )
		: _graph( graph )
	{}

	inline bool operator()( const edge_descriptor& ed1, const edge_descriptor& ed2 ) const
	{
		const Vertex& v1 = _graph.targetInstance( ed1 );
		const Vertex& v2 = _graph.targetInstance( ed2 );

		bool res= v1.getProcessDataAtTime()._globalInfos._memory < v2.getProcessDataAtTime()._globalInfos._memory;
//		COUT_VAR2(v1.getName(), v1.getProcessDataAtTime()._globalInfos._memory);
//		COUT_VAR2(v2.getName(), v2.getProcessDataAtTime()._globalInfos._memory);
//		COUT_VAR(res);
		return res;
	}
private:
	const TGraph& _graph;
};
*/

memory::MemoryCache ProcessGraph::process( const int tBegin, const int tEnd )
{
	using namespace boost;
	using namespace boost::graph;
#ifndef TUTTLE_PRODUCTION
	TCOUT( "process" );
	graph::exportAsDOT( "graphprocess.dot", _graph );
#endif

	// Initialize variables
	OfxPointD renderScale = { 1.0, 1.0 };
	OfxRectD renderWindow = { 0, 0, 0, 0 };

	//--- BEGIN RENDER
	ProcessVertexData defaultOptions;
	defaultOptions._renderTimeRange.x = tBegin;
	defaultOptions._renderTimeRange.y = tEnd;
	defaultOptions._step        = 1;
	defaultOptions._interactive = false;
	// imageEffect specific...
//	defaultOptions._field       = kOfxImageFieldBoth;
	defaultOptions._renderScale = renderScale;
//	defaultOptions._renderRoI   = renderWindow;

	///@todo tuttle: exception if there is non-optional clips unconnected.
//	graph::visitor::UnconnectedClips<InternalGraphImpl> unconnectedClipsVisitor( renderGraph );
//	renderGraph.depthFirstSearch( unconnectedClipsVisitor );
//	if( unconnectedClipsVisitor.value )
//	{
//		exception::user userMsg("Some non optional clips are unconnected. We can't do the process.\n");
//		userMsg << "Unconnected clips : ";
//		BOOST_FOREACH( clip, unconnectedClipsVisitor.clips )
//		{
//			userMsg << clip->getFullName() << ",";
//		}
//		userMsg << std::endl;
//		BOOST_THROW_EXCEPTION( exception::Logic()
//			<< userMsg );
//	}

	BOOST_FOREACH( InternalGraphImpl::vertex_descriptor vd, _graph.getVertices() )
	{
		Vertex& v = _graph.instance(vd);
		if( ! v.isFake() )
		{
			v.setProcessData( defaultOptions );
			v.getProcessNode().setProcessData( &v._data );
		}
	}

	///@todo tuttle: compute the time domain for each node
//	graph::visitor::TimeDomain<InternalGraphImpl> timeDomainVisitor( renderGraph );
//	renderGraph.depthFirstSearch( timeDomainVisitor );
	
	TCOUT( "process begin" );
//	BOOST_FOREACH( NodeMap::value_type& p, _nodes )
	for( NodeMap::iterator it = _nodes.begin(), itEnd = _nodes.end();
	     it != itEnd;
	     ++it )
	{
		NodeMap::value_type& p = *it;
		p.second->beginSequence( defaultOptions );
	}

	memory::MemoryCache result;
	TCOUT( "process render..." );
	//--- RENDER
	// at each frame
	for( int time = tBegin; time <= tEnd; ++time )
	{
		TCOUT( "________________________________________ frame: " << time );
		// use an internal copy for inside the process
		InternalGraphImpl renderGraph = _graph;
		TCOUT( "________________________________________ output node : " << renderGraph.getVertex( _outputId ).getName() );

		TCOUT( "---------------------------------------- connect clips" );
		connectClips<InternalGraphImpl>( renderGraph );

		TCOUT( "---------------------------------------- deploy time" );
		//fill renderGraph from _graph with each node deployed at all times
		graph::visitor::DeployTime<InternalGraphImpl> deployTimeVisitor( renderGraph, time );
		renderGraph.depthFirstSearchReverse( deployTimeVisitor );
#ifndef TUTTLE_PRODUCTION
		graph::exportDebugAsDOT( "graphprocess_a.dot", renderGraph );
#endif

		TCOUT( "---------------------------------------- build renderGraphAtTime" );
		// create a new graph with time information
		InternalGraphAtTimeImpl renderGraphAtTime;
		BOOST_FOREACH( InternalGraphAtTimeImpl::vertex_descriptor vd, renderGraph.getVertices() )
		{
			Vertex& v = renderGraph.instance( vd );
			//BOOST_FOREACH( const OfxTime t, v._data._times )
            for( ProcessVertexData::TimesSet::iterator it = v._data._times.begin(), itEnd = v._data._times.end();
                 it != itEnd;
                 ++it )
			{
                const OfxTime t = *it;
				TCOUT_VAR2( v, t );
				renderGraphAtTime.addVertex( ProcessVertexAtTime(v, t) );
			}
		}
		BOOST_FOREACH( InternalGraphAtTimeImpl::edge_descriptor ed, renderGraph.getEdges() )
		{
			Edge& e = renderGraph.instance( ed );
			Vertex& in = renderGraph.sourceInstance( ed );
			Vertex& out = renderGraph.targetInstance( ed );
			TCOUT_X( 20, "." );
			TCOUT_VAR( e );
			BOOST_FOREACH( const Edge::TimeMap::value_type& tm, e._timesNeeded )
			{
				BOOST_FOREACH( const OfxTime t2, tm.second )
				{
					TCOUT_VAR2( in.getKey(), tm.first );
					TCOUT_VAR2( out.getKey(), t2 );
					TCOUT_VAR( e.getInAttrName() );
					renderGraphAtTime.connect(
						ProcessVertexAtTime::Key(in.getKey(), tm.first),
						ProcessVertexAtTime::Key(out.getKey(), t2),
						e.getInAttrName() );
				}
			}
		}
		BOOST_FOREACH( InternalGraphAtTimeImpl::vertex_descriptor vd, renderGraphAtTime.getVertices() )
		{
			ProcessVertexAtTime& v = renderGraphAtTime.instance(vd);
			if( ! v.isFake() )
			{
				v.getProcessNode().setProcessData( &v._data );
			}
		}

		ProcessVertexAtTime::Key outputKeyAtTime(_outputId, time);
		InternalGraphAtTimeImpl::vertex_descriptor& outputAtTime = renderGraphAtTime.getVertexDescriptor( outputKeyAtTime );

		TCOUT( "---------------------------------------- set default options" );
		BOOST_FOREACH( InternalGraphAtTimeImpl::vertex_descriptor vd, renderGraphAtTime.getVertices() )
		{
			ProcessVertexAtTime& v = renderGraphAtTime.instance( vd );

			v.getProcessDataAtTime()._inDegree  = renderGraphAtTime.getInDegree( vd );
			v.getProcessDataAtTime()._outDegree = renderGraphAtTime.getOutDegree( vd );
		}
		// for each final nodes
		BOOST_FOREACH( InternalGraphAtTimeImpl::edge_descriptor ed, boost::out_edges( outputAtTime, renderGraphAtTime.getGraph() ) )
		{
			ProcessVertexAtTime& v = renderGraphAtTime.targetInstance( ed );
			v.getProcessDataAtTime()._finalNode = true;
		}
#ifndef TUTTLE_PRODUCTION
		graph::exportDebugAsDOT( "graphprocess_b.dot", renderGraphAtTime );
#endif
		
		TCOUT( "---------------------------------------- preprocess 1" );
		graph::visitor::PreProcess1<InternalGraphAtTimeImpl> preProcess1Visitor( renderGraphAtTime );
		renderGraphAtTime.depthFirstVisit( preProcess1Visitor, outputAtTime );

		TCOUT( "---------------------------------------- preprocess 2" );
		graph::visitor::PreProcess2<InternalGraphAtTimeImpl> preProcess2Visitor( renderGraphAtTime );
		renderGraphAtTime.depthFirstSearchReverse( preProcess2Visitor );

		TCOUT( "---------------------------------------- preprocess 3" );
		graph::visitor::PreProcess3<InternalGraphAtTimeImpl> preProcess3Visitor( renderGraphAtTime );
		renderGraphAtTime.depthFirstVisit( preProcess3Visitor, outputAtTime );

//		TCOUT( "---------------------------------------- remove identity nodes" );
//		graph::visitor::RemoveIdentityNodes<InternalGraphAtTimeImpl> removeIdentityVisitor( renderGraphAtTime );
//		renderGraphAtTime.depthFirstVisit( removeIdentityVisitor, outputAtTime );

#ifndef TUTTLE_PRODUCTION
		graph::exportDebugAsDOT( "graphprocess_c.dot", renderGraphAtTime );
#endif

		TCOUT( "---------------------------------------- reconnect clips" );
		connectClips<InternalGraphAtTimeImpl>( renderGraphAtTime );
		
		TCOUT( "---------------------------------------- optimize graph" );
		graph::visitor::OptimizeGraph<InternalGraphAtTimeImpl> optimizeGraphVisitor( renderGraphAtTime );
		renderGraphAtTime.depthFirstVisit( optimizeGraphVisitor, outputAtTime );
#ifndef TUTTLE_PRODUCTION
		graph::exportDebugAsDOT( "graphprocess_d.dot", renderGraphAtTime );
#endif
		
		/*
		InternalGraphImpl tmpGraph;
		output = renderGraph.getVertexDescriptor( _outputId );
		/// @todo tuttle: out_edges sort don't work...
		TCOUT( "---------------------------------------- sorting graph" );
		BOOST_FOREACH( InternalGraphImpl::vertex_descriptor vd, renderGraph.getVertices() )
		{
			std::vector<InternalGraphImpl::Edge> edges( boost::out_degree(vd, renderGraph.getGraph()) );

			BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( vd, renderGraph.getGraph() ) )
			{
				edges.push_back( renderGraph.instance(ed) );
			}

			Vertex& v = renderGraph.instance(vd);
			COUT_X( 30, "-" );
			std::size_t i = 0;
			COUT( "before sort edges of " << v.getName() );
			BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( vd, renderGraph.getGraph() ) )
			{
				Edge& e = renderGraph.instance(ed);
				e._localId = i++;
				e._name += " -- ";
				e._name += boost::lexical_cast<std::string>(e._localId); // tmp
				COUT( e.getName() << " - " <<  renderGraph.targetInstance(ed).getProcessDataAtTime()._globalInfos._memory  );
			}
			std::sort( edges.begin(), edges.end(), SortEdgeByMemorySize<InternalGraphImpl>(renderGraph) );
			COUT( "after sort edges of " << v.getName() );
			BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( vd, renderGraph.getGraph() ) )
			{
				Edge& e = renderGraph.instance(ed);
				COUT( e.getName() << " - " <<  renderGraph.targetInstance(ed).getProcessDataAtTime()._globalInfos._memory );
			}
			InternalGraphImpl::out_edge_iterator oe_it, oe_itEnd;
			boost::tie( oe_it, oe_itEnd ) = boost::out_edges( vd, renderGraph.getGraph() );
			for( ; oe_it != oe_itEnd; ++oe_it )
			{
				Edge& e = renderGraph.instance(*oe_it);
				COUT( e.getName() << " - " <<  renderGraph.targetInstance(*oe_it).getProcessDataAtTime()._globalInfos._memory );
			}
		}
#ifndef TUTTLE_PRODUCTION
		graph::exportDebugAsDOT( "graphprocess_d.dot", tmpGraph );
#endif
		*/

		TCOUT( "---------------------------------------- process" );
		graph::visitor::Process<InternalGraphAtTimeImpl> processVisitor( renderGraphAtTime, Core::instance().getMemoryCache(), result );
		renderGraphAtTime.depthFirstVisit( processVisitor, outputAtTime );

		TCOUT( "---------------------------------------- postprocess" );
		graph::visitor::PostProcess<InternalGraphAtTimeImpl> postProcessVisitor( renderGraphAtTime );
		renderGraphAtTime.depthFirstVisit( postProcessVisitor, outputAtTime );

		// end of one frame
		// do some clean: memory clean, as temporary solution...
		TCOUT( "---------------------------------------- clearUnused" );
		Core::instance().getMemoryCache().clearUnused();
		TCOUT_VAR( Core::instance().getMemoryCache().size() );
		TCOUT_VAR( Core::instance().getMemoryCache() );
		TCOUT_VAR( result );

	}

	//--- END RENDER
	BOOST_FOREACH( NodeMap::value_type& p, _nodes )
	{
		p.second->endSequence( defaultOptions ); // node option... or no option here ?
	}
	return result;
}

}
}
}

