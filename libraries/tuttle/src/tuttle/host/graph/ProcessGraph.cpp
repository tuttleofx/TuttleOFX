#include "ProcessGraph.hpp"
#include "ProcessVisitors.hpp"
#include <tuttle/common/utils/color.hpp>
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

	if( outputNodes.size() )
	{
		BOOST_FOREACH( const std::string & s, outputNodes )
		{
			_graph.connect( _outputId, s, "Output" );
			TUTTLE_COUT_DEBUG( "MY OUTPUT: " << s );
		}
	}
	else
	{
		// Detect root nodes and add them to the list of nodes to process
		BOOST_FOREACH( const InternalGraphImpl::vertex_descriptor vd, _graph.rootVertices() )
		{
			InternalGraphImpl::VertexKey vk = _graph.instance( vd ).getKey();
			_graph.connect( _outputId, vk, "Output" );
		}
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
//		TUTTLE_COUT_VAR2(v1.getName(), v1.getProcessDataAtTime()._globalInfos._memory);
//		TUTTLE_COUT_VAR2(v2.getName(), v2.getProcessDataAtTime()._globalInfos._memory);
//		TUTTLE_COUT_VAR(res);
		return res;
	}
private:
	const TGraph& _graph;
};
*/

void ProcessGraph::bakeGraphInformationToNodes( InternalGraphAtTimeImpl& renderGraphAtTime )
{
	TUTTLE_TCOUT( "---------------------------------------- set default options" ); // doublon...
	BOOST_FOREACH( const InternalGraphAtTimeImpl::vertex_descriptor vd, renderGraphAtTime.getVertices() )
	{
		VertexAtTime& v = renderGraphAtTime.instance( vd );
		ProcessVertexAtTimeData& vData = v.getProcessDataAtTime();
		TUTTLE_TCOUT( "\n--- node: " << v.getName() );

		vData._outDegree  = renderGraphAtTime.getInDegree( vd ) - vData._isFinalNode;
		vData._inDegree = renderGraphAtTime.getOutDegree( vd );

		vData._outEdges.reserve( vData._outDegree );
		vData._outEdges.clear();
		BOOST_FOREACH( const InternalGraphAtTimeImpl::edge_descriptor ed, renderGraphAtTime.getInEdges( vd ) )
		{
			TUTTLE_TCOUT( " - a" );
			const ProcessEdgeAtTime* e = &renderGraphAtTime.instance(ed);
			VertexAtTime& v = renderGraphAtTime.sourceInstance( ed );
			if( v.isFake() )
				continue;
			TUTTLE_TCOUT_VAR( e );
			TUTTLE_TCOUT_VAR( e->getInTime() );
			TUTTLE_TCOUT_VAR( e->getInAttrName() );
			vData._outEdges.push_back( e );
		}
		vData._inEdges.reserve( vData._inDegree );
		vData._inEdges.clear();
		BOOST_FOREACH( const InternalGraphAtTimeImpl::edge_descriptor ed, renderGraphAtTime.getOutEdges( vd ) )
		{
			TUTTLE_TCOUT( " - b" );
			const ProcessEdgeAtTime* e = &renderGraphAtTime.instance(ed);
			TUTTLE_TCOUT_VAR( e );
			TUTTLE_TCOUT_VAR( e->getInAttrName() );
			vData._inEdges.push_back( e );
		}
	}
	TUTTLE_TCOUT( "---------------------------------------- connect clips" );
	connectClips<InternalGraphAtTimeImpl>( renderGraphAtTime );

}

void ProcessGraph::beginSequenceRender( ProcessVertexData& procOptions )
{
	TUTTLE_TCOUT( "process begin sequence" );
	//	BOOST_FOREACH( NodeMap::value_type& p, _nodes )
	for( NodeMap::iterator it = _nodes.begin(), itEnd = _nodes.end();
		it != itEnd;
		++it )
	{
		NodeMap::value_type& p = *it;
		p.second->beginSequence( procOptions );
	}
}

void ProcessGraph::endSequenceRender( ProcessVertexData& procOptions )
{
	TUTTLE_TCOUT( "process end sequence" );
	//--- END sequence render
	BOOST_FOREACH( NodeMap::value_type& p, _nodes )
	{
		p.second->endSequence( procOptions ); // node option... or no option here ?
	}
}

void ProcessGraph::process( memory::MemoryCache& result, const ComputeOptions& options )
{
	using namespace boost;
	using namespace boost::graph;
#ifndef TUTTLE_PRODUCTION
	TUTTLE_TCOUT( "process" );
	graph::exportAsDOT( "graphProcess_a.dot", _graph );
#endif

	// Initialize variables
//	OfxRectD renderWindow = { 0, 0, 0, 0 };

	//--- BEGIN RENDER
	ProcessVertexData procOptions;
	procOptions._interactive = options.getIsInteractive();
	// imageEffect specific...
//	procOptions._field       = kOfxImageFieldBoth;
	procOptions._renderScale = options.getRenderScale();
//	procOptions._renderRoI   = renderWindow;

	///@todo tuttle: exception if there is non-optional clips unconnected.
	/// It's already checked in the beginSequence of the imageEffectNode.
	/// But maybe it could better to check that here independently from node types.
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
	
	// use an internal copy for inside the process
	InternalGraphImpl renderGraph = _graph;

	BOOST_FOREACH( InternalGraphImpl::vertex_descriptor vd, renderGraph.getVertices() )
	{
		Vertex& v = renderGraph.instance(vd);
		if( ! v.isFake() )
		{
			v.setProcessData( procOptions );
			v.getProcessNode().setProcessData( &v._data );
		}
	}
	
	TUTTLE_TCOUT( "---------------------------------------- Connect clips" );
	connectClips<InternalGraphImpl>( renderGraph );
	
	TUTTLE_TCOUT( "---------------------------------------- Time Domain propagation" );
	graph::visitor::TimeDomain<InternalGraphImpl> timeDomainPropagationVisitor( renderGraph );
	renderGraph.depthFirstVisit( timeDomainPropagationVisitor, renderGraph.getVertexDescriptor( _outputId ) );

	TUTTLE_TCOUT_INFOS;
	std::list<TimeRange> timeRanges = options.getTimeRanges();

	TUTTLE_TCOUT_INFOS;
	if( timeRanges.empty() )
	{
		BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( renderGraph.getVertexDescriptor(_outputId), renderGraph.getGraph() ) )
		{
			TUTTLE_TCOUT_INFOS;
			ProcessVertex& v = renderGraph.targetInstance( ed );
			// compute the time domain for each output node
			TUTTLE_TCOUT_INFOS;
			OfxRangeD timeDomain = v.getProcessData()._timeDomain;

			TUTTLE_TCOUT_VAR2( timeDomain.min, timeDomain.max );
			// special case for infinite time domain (eg. a still image)
			if( timeDomain.min <= kOfxFlagInfiniteMin )
				timeDomain.min = 0;
			if( timeDomain.max >= kOfxFlagInfiniteMax )
				timeDomain.max = 0;

			TUTTLE_TCOUT_INFOS;
			timeRanges.push_back( TimeRange( timeDomain ) );
			TUTTLE_TCOUT_INFOS;
			TUTTLE_TCOUT( "Compute " << quotes(v.getName()) << " full time domain: from " << timeDomain.min << " to " << timeDomain.max << "." );
		}
	}
	TUTTLE_TCOUT_INFOS;

	/// @todo Bug: need to use a map 'OutputNode': 'timeRanges'
	/// And check if all Output nodes share a common timeRange
	
	TUTTLE_TCOUT( "process render..." );
	//--- RENDER
	// at each frame
	BOOST_FOREACH( const TimeRange& timeRange, timeRanges )
	{
		procOptions._renderTimeRange.min = timeRange._begin;
		procOptions._renderTimeRange.max = timeRange._end;
		procOptions._step                = timeRange._step;

		beginSequenceRender( procOptions );
		
		for( int time = timeRange._begin; time <= timeRange._end; time += timeRange._step )
		{
			if( options.getAbort() )
			{
				endSequenceRender( procOptions );
				return;
			}
			
			try
			{
				TUTTLE_COUT( tuttle::common::kColorBlue << "process at time " << time << tuttle::common::kColorStd );
				TUTTLE_TCOUT( "________________________________________ frame: " << time );
	
				TUTTLE_TCOUT( "________________________________________ output node : " << renderGraph.getVertex( _outputId ).getName() );
				
				TUTTLE_TCOUT( "---------------------------------------- deploy time" );
				graph::visitor::DeployTime<InternalGraphImpl> deployTimeVisitor( renderGraph, time );
				renderGraph.depthFirstSearchReverse( deployTimeVisitor );
		#ifndef TUTTLE_PRODUCTION
				graph::exportDebugAsDOT( "graphProcess_b.dot", renderGraph );
		#endif

				TUTTLE_TCOUT( "---------------------------------------- build renderGraphAtTime" );
				// create a new graph with time information
				InternalGraphAtTimeImpl renderGraphAtTime;
				{
					BOOST_FOREACH( InternalGraphAtTimeImpl::vertex_descriptor vd, renderGraph.getVertices() )
					{
						Vertex& v = renderGraph.instance( vd );
						BOOST_FOREACH( const OfxTime t, v._data._times )
						{
							TUTTLE_TCOUT_VAR2( v, t );
							renderGraphAtTime.addVertex( ProcessVertexAtTime(v, t) );
						}
					}
					BOOST_FOREACH( const InternalGraphAtTimeImpl::edge_descriptor ed, renderGraph.getEdges() )
					{
						const Edge& e = renderGraph.instance( ed );
						const Vertex& in = renderGraph.sourceInstance( ed );
						const Vertex& out = renderGraph.targetInstance( ed );
						TUTTLE_TCOUT_X( 20, "." );
						TUTTLE_TCOUT_VAR( e );
						BOOST_FOREACH( const Edge::TimeMap::value_type& tm, e._timesNeeded )
						{
							const VertexAtTime procIn( in, tm.first );
							BOOST_FOREACH( const OfxTime t2, tm.second )
							{
								//TUTTLE_TCOUT_X( 10, "," );
								//TUTTLE_TCOUT_VAR( tm.first );
								//TUTTLE_TCOUT_VAR( t2 );
								const VertexAtTime procOut( out, t2 );

								const VertexAtTime::Key inKey( procIn.getKey() );
								const VertexAtTime::Key outKey( procOut.getKey() );

								//TUTTLE_TCOUT_VAR( inKey );
								//TUTTLE_TCOUT_VAR( outKey );
								//TUTTLE_TCOUT_VAR( e.getInAttrName() );

								const EdgeAtTime eAtTime( outKey, inKey, e.getInAttrName() );

								renderGraphAtTime.addEdge(
									renderGraphAtTime.getVertexDescriptor( inKey ),
									renderGraphAtTime.getVertexDescriptor( outKey ),
									eAtTime );
							}
						}
					}
				}
				
				VertexAtTime::Key outputKeyAtTime( VertexAtTime(Vertex(_outputId), time).getKey() );
				InternalGraphAtTimeImpl::vertex_descriptor outputAtTime = renderGraphAtTime.getVertexDescriptor( outputKeyAtTime );
				// declare final nodes
				BOOST_FOREACH( const InternalGraphAtTimeImpl::edge_descriptor ed, boost::out_edges( outputAtTime, renderGraphAtTime.getGraph() ) )
				{
					VertexAtTime& v = renderGraphAtTime.targetInstance( ed );
					v.getProcessDataAtTime()._isFinalNode = true; /// @todo: this is maybe better to move this into the ProcessData? Doesn't depend on time?
				}
				
				TUTTLE_TCOUT( "---------------------------------------- set data at time" );
				// give a link to the node on its attached process data
				BOOST_FOREACH( const InternalGraphAtTimeImpl::vertex_descriptor vd, renderGraphAtTime.getVertices() )
				{
					VertexAtTime& v = renderGraphAtTime.instance(vd);
					if( ! v.isFake() )
					{
						//TUTTLE_TCOUT( "setProcessDataAtTime: " << v._name << " id: " << v._id << " at time: " << v._data._time );
						v.getProcessNode().setProcessDataAtTime( &v._data );
					}
				}
				
				bakeGraphInformationToNodes( renderGraphAtTime );

				
		#ifndef TUTTLE_PRODUCTION
				graph::exportDebugAsDOT( "graphProcessAtTime_a.dot", renderGraphAtTime );
		#endif

				{
					TUTTLE_TCOUT( "---------------------------------------- preprocess 1" );
					TUTTLE_TCOUT_INFOS;
					graph::visitor::PreProcess1<InternalGraphAtTimeImpl> preProcess1Visitor( renderGraphAtTime );
					TUTTLE_TCOUT_INFOS;
					renderGraphAtTime.depthFirstVisit( preProcess1Visitor, outputAtTime );
					TUTTLE_TCOUT_INFOS;
				}

				{
					TUTTLE_TCOUT( "---------------------------------------- preprocess 2" );
					graph::visitor::PreProcess2<InternalGraphAtTimeImpl> preProcess2Visitor( renderGraphAtTime );
					renderGraphAtTime.depthFirstVisit( preProcess2Visitor, outputAtTime );
				}

				{
					TUTTLE_TCOUT( "---------------------------------------- preprocess 3" );
					graph::visitor::PreProcess3<InternalGraphAtTimeImpl> preProcess3Visitor( renderGraphAtTime );
					renderGraphAtTime.depthFirstVisit( preProcess3Visitor, outputAtTime );
				}
				
		#ifndef TUTTLE_PRODUCTION
				graph::exportDebugAsDOT( "graphProcessAtTime_b.dot", renderGraphAtTime );
		#endif

				if( ! options.getForceIdentityNodesProcess() )
				{
					TUTTLE_TCOUT( "---------------------------------------- remove identity nodes" );
					// The "Remove identity nodes" step need to be done after preprocess steps, because the RoI need to be computed.
					std::vector<graph::visitor::IdentityNodeConnection<InternalGraphAtTimeImpl> > toRemove;
					
					graph::visitor::RemoveIdentityNodes<InternalGraphAtTimeImpl> vis( renderGraphAtTime, toRemove );
					renderGraphAtTime.depthFirstVisit( vis, outputAtTime );
					TUTTLE_TCOUT_VAR( toRemove.size() );
					if( toRemove.size() )
					{
						graph::visitor::removeIdentityNodes( renderGraphAtTime, toRemove );
						
						// Bake graph information again as the connections have changed.
						bakeGraphInformationToNodes( renderGraphAtTime );
					}
				}

				/*
				TUTTLE_TCOUT( "---------------------------------------- optimize graph" );
				graph::visitor::OptimizeGraph<InternalGraphAtTimeImpl> optimizeGraphVisitor( renderGraphAtTime );
				renderGraphAtTime.depthFirstVisit( optimizeGraphVisitor, outputAtTime );
				*/
		#ifndef TUTTLE_PRODUCTION
				graph::exportDebugAsDOT( "graphProcessAtTime_c.dot", renderGraphAtTime );
		#endif
				/*
				InternalGraphImpl tmpGraph;
				output = renderGraph.getVertexDescriptor( _outputId );
				/// @todo tuttle: out_edges sort don't work...
				TUTTLE_TCOUT( "---------------------------------------- sorting graph" );
				BOOST_FOREACH( InternalGraphImpl::vertex_descriptor vd, renderGraph.getVertices() )
				{
					std::vector<InternalGraphImpl::Edge> edges( boost::out_degree(vd, renderGraph.getGraph()) );

					BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( vd, renderGraph.getGraph() ) )
					{
						edges.push_back( renderGraph.instance(ed) );
					}

					Vertex& v = renderGraph.instance(vd);
					TUTTLE_TCOUT_X( 30, "-" );
					std::size_t i = 0;
					TUTTLE_TCOUT( "before sort edges of " << v.getName() );
					BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( vd, renderGraph.getGraph() ) )
					{
						Edge& e = renderGraph.instance(ed);
						e._localId = i++;
						e._name += " -- ";
						e._name += boost::lexical_cast<std::string>(e._localId); // tmp
						TUTTLE_TCOUT( e.getName() << " - " <<  renderGraph.targetInstance(ed).getProcessDataAtTime()._globalInfos._memory  );
					}
					std::sort( edges.begin(), edges.end(), SortEdgeByMemorySize<InternalGraphImpl>(renderGraph) );
					TUTTLE_TCOUT( "after sort edges of " << v.getName() );
					BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( vd, renderGraph.getGraph() ) )
					{
						Edge& e = renderGraph.instance(ed);
						TUTTLE_TCOUT( e.getName() << " - " <<  renderGraph.targetInstance(ed).getProcessDataAtTime()._globalInfos._memory );
					}
					InternalGraphImpl::out_edge_iterator oe_it, oe_itEnd;
					boost::tie( oe_it, oe_itEnd ) = boost::out_edges( vd, renderGraph.getGraph() );
					for( ; oe_it != oe_itEnd; ++oe_it )
					{
						Edge& e = renderGraph.instance(*oe_it);
						TUTTLE_TCOUT( e.getName() << " - " <<  renderGraph.targetInstance(*oe_it).getProcessDataAtTime()._globalInfos._memory );
					}
				}
		#ifndef TUTTLE_PRODUCTION
				graph::exportDebugAsDOT( "graphprocess_d.dot", tmpGraph );
		#endif
				*/

				TUTTLE_TCOUT( "---------------------------------------- process" );
				// do the process
				graph::visitor::Process<InternalGraphAtTimeImpl> processVisitor( renderGraphAtTime, Core::instance().getMemoryCache() );
				if( options.getReturnBuffers() )
				{
					// accumulate output nodes buffers into the @p result MemoryCache
					processVisitor.setOutputMemoryCache( result );
				}

				renderGraphAtTime.depthFirstVisit( processVisitor, outputAtTime );

				TUTTLE_TCOUT( "---------------------------------------- postprocess" );
				graph::visitor::PostProcess<InternalGraphAtTimeImpl> postProcessVisitor( renderGraphAtTime );
				renderGraphAtTime.depthFirstVisit( postProcessVisitor, outputAtTime );

				// end of one frame
				// do some clean: memory clean, as temporary solution...
				TUTTLE_TCOUT( "---------------------------------------- clearUnused" );
				Core::instance().getMemoryCache().clearUnused();
				TUTTLE_TCOUT_VAR( Core::instance().getMemoryCache().size() );
				TUTTLE_TCOUT_VAR( Core::instance().getMemoryCache() );
				TUTTLE_TCOUT_VAR( result );

				TUTTLE_COUT( " " );
			}
			catch( tuttle::exception::FileNotExist& e ) // @todo tuttle: change that.
			{
				if( options.getContinueOnError() && ! options.getAbort() )
				{
					TUTTLE_COUT( tuttle::common::kColorError << "Undefined input at time " << time << "." << tuttle::common::kColorStd << "\n" );
	#ifndef TUTTLE_PRODUCTION
					TUTTLE_COUT_ERROR( boost::diagnostic_information(e) );
	#endif
				}
				else
				{
					endSequenceRender( procOptions );
					throw;
				}
			}
			catch( ... )
			{
				if( options.getContinueOnError() && ! options.getAbort() )
				{
					TUTTLE_COUT( tuttle::common::kColorError << "Skip frame " << time << "." << tuttle::common::kColorStd );
#ifndef TUTTLE_PRODUCTION
					TUTTLE_COUT_ERROR( "Skip frame " << time << "." );
					TUTTLE_COUT( boost::current_exception_diagnostic_information() );
#endif
				}
				else
				{
					endSequenceRender( procOptions );
					throw;
				}
			}
		}
		
		endSequenceRender( procOptions );
	}
}

}
}
}

