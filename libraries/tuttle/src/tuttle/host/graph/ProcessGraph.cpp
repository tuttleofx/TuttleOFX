#include "ProcessGraph.hpp"
#include "ProcessVisitors.hpp"
#include <tuttle/common/utils/color.hpp>
#include <tuttle/host/graph/GraphExporter.hpp>

#include <boost/foreach.hpp>
//#include <boost/timer/timer.hpp>

namespace tuttle {
namespace host {
namespace graph {

const std::string ProcessGraph::_outputId( "TUTTLE_FAKE_OUTPUT" );

ProcessGraph::ProcessGraph( const ComputeOptions& options, Graph& userGraph, const std::list<std::string>& outputNodes )
	: _instanceCount( userGraph.getInstanceCount() )
	, _options(options)
{
	updateGraph( userGraph, outputNodes );
}

ProcessGraph::~ProcessGraph()
{}


ProcessGraph::VertexAtTime::Key ProcessGraph::getOutputKeyAtTime( const OfxTime time )
{
	return VertexAtTime(Vertex(_outputId), time).getKey();
}
ProcessGraph::InternalGraphAtTimeImpl::vertex_descriptor ProcessGraph::getOutputVertexAtTime( const OfxTime time )
{
	return _renderGraphAtTime.getVertexDescriptor( getOutputKeyAtTime( time ) );
}

/**
 * @brief After copying Vertices, we need to duplicate Nodes and relink Vertices with new Nodes.
 */
void ProcessGraph::relink()
{
	_renderGraph.removeUnconnectedVertices( _renderGraph.getVertexDescriptor( _outputId ) );

	BOOST_FOREACH( InternalGraphImpl::vertex_descriptor vd, _renderGraph.getVertices() )
	{
		Vertex& v = _renderGraph.instance( vd );

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

    _renderGraph.removeVertex( v );
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
		: _renderGraph( graph )
	{}

	inline bool operator()( const edge_descriptor& ed1, const edge_descriptor& ed2 ) const
	{
		const Vertex& v1 = _renderGraph.targetInstance( ed1 );
		const Vertex& v2 = _renderGraph.targetInstance( ed2 );

		bool res= v1.getProcessDataAtTime()._globalInfos._memory < v2.getProcessDataAtTime()._globalInfos._memory;
//		TUTTLE_COUT_VAR2(v1.getName(), v1.getProcessDataAtTime()._globalInfos._memory);
//		TUTTLE_COUT_VAR2(v2.getName(), v2.getProcessDataAtTime()._globalInfos._memory);
//		TUTTLE_COUT_VAR(res);
		return res;
	}
private:
	const TGraph& _renderGraph;
};
*/

void ProcessGraph::bakeGraphInformationToNodes( InternalGraphAtTimeImpl& _renderGraphAtTime )
{
	TUTTLE_TCOUT( "---------------------------------------- set default options" ); // doublon...
	BOOST_FOREACH( const InternalGraphAtTimeImpl::vertex_descriptor vd, _renderGraphAtTime.getVertices() )
	{
		VertexAtTime& v = _renderGraphAtTime.instance( vd );
		ProcessVertexAtTimeData& vData = v.getProcessDataAtTime();
		TUTTLE_TCOUT( "\n--- node: " << v.getName() );

		vData._outDegree = _renderGraphAtTime.getInDegree( vd ) - vData._isFinalNode;
		vData._inDegree = _renderGraphAtTime.getOutDegree( vd );

		vData._outEdges.clear();
		vData._outEdges.reserve( vData._outDegree );
		BOOST_FOREACH( const InternalGraphAtTimeImpl::edge_descriptor ed, _renderGraphAtTime.getInEdges( vd ) )
		{
			TUTTLE_TCOUT( " - a" );
			const ProcessEdgeAtTime* e = &_renderGraphAtTime.instance(ed);
			VertexAtTime& v = _renderGraphAtTime.sourceInstance( ed );
			if( v.isFake() )
				continue;
			TUTTLE_TCOUT_VAR( e );
			TUTTLE_TCOUT_VAR( e->getInTime() );
			TUTTLE_TCOUT_VAR( e->getInAttrName() );
			vData._outEdges.push_back( e );
		}
		vData._inEdges.clear();
		BOOST_FOREACH( const InternalGraphAtTimeImpl::edge_descriptor ed, _renderGraphAtTime.getOutEdges( vd ) )
		{
			TUTTLE_TCOUT( " - b" );
			const ProcessEdgeAtTime* e = &_renderGraphAtTime.instance(ed);
			TUTTLE_TCOUT_VAR( e );
			TUTTLE_TCOUT_VAR( e->getInAttrName() );
			vData._inEdges[e->getInAttrName()] = e;
		}
	}
	TUTTLE_TCOUT( "---------------------------------------- connect clips" );
	connectClips<InternalGraphAtTimeImpl>( _renderGraphAtTime );

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

void ProcessGraph::updateGraph( Graph& userGraph, const std::list<std::string>& outputNodes )
{
	_renderGraph.copyTransposed( userGraph.getGraph() );

	Vertex outputVertex( _outputId );

	if( outputNodes.size() )
	{
		_renderGraph.addVertex( outputVertex );
		BOOST_FOREACH( const std::string & s, outputNodes )
		{
			_renderGraph.connect( _outputId, s, "Output" );
			TUTTLE_COUT_DEBUG( "MY OUTPUT: " << s );
		}
	}
	else
	{
		// Detect root nodes and add them to the list of nodes to process
		std::vector<InternalGraphImpl::vertex_descriptor> rootVertices = _renderGraph.rootVertices();
		_renderGraph.addVertex( outputVertex );
		BOOST_FOREACH( const InternalGraphImpl::vertex_descriptor vd, rootVertices )
		{
			InternalGraphImpl::VertexKey vk = _renderGraph.instance( vd ).getKey();
			_renderGraph.connect( _outputId, vk, "Output" );
		}
	}
	
	relink();
}

void ProcessGraph::setup()
{
	using namespace boost;
	using namespace boost::graph;
	TUTTLE_TCOUT( "process" );
	
	// Initialize variables
//	OfxRectD renderWindow = { 0, 0, 0, 0 };

	//--- BEGIN RENDER
	ProcessVertexData defaultVertexData;
	defaultVertexData._interactive = _options.getIsInteractive();
	// imageEffect specific...
//	defaultVertexData._field       = kOfxImageFieldBoth;
	defaultVertexData._renderScale = _options.getRenderScale();
//	defaultVertexData._renderRoI   = renderWindow;

	///@todo tuttle: exception if there is non-optional clips unconnected.
	/// It's already checked in the beginSequence of the imageEffectNode.
	/// But maybe it could better to check that here independently from node types.
//	graph::visitor::UnconnectedClips<InternalGraphImpl> unconnectedClipsVisitor( _renderGraph );
//	_renderGraph.depthFirstSearch( unconnectedClipsVisitor );
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

	BOOST_FOREACH( InternalGraphImpl::vertex_descriptor vd, _renderGraph.getVertices() )
	{
		Vertex& v = _renderGraph.instance(vd);
		if( ! v.isFake() )
		{
			v.setProcessData( defaultVertexData );
			v.getProcessNode().setProcessData( &v._data );
		}
	}
	
	TUTTLE_TCOUT( "---------------------------------------- Connect clips" );
	connectClips<InternalGraphImpl>( _renderGraph );

	{	
		TUTTLE_TCOUT( "---------------------------------------- Setup" );
		graph::visitor::Setup1<InternalGraphImpl> setup1Visitor( _renderGraph );
		_renderGraph.depthFirstVisit( setup1Visitor, _renderGraph.getVertexDescriptor( _outputId ) );
		graph::visitor::Setup2<InternalGraphImpl> setup2Visitor( _renderGraph );
		_renderGraph.depthFirstVisit( setup2Visitor, _renderGraph.getVertexDescriptor( _outputId ) );
		graph::visitor::Setup3<InternalGraphImpl> setup3Visitor( _renderGraph );
		_renderGraph.depthFirstVisit( setup3Visitor, _renderGraph.getVertexDescriptor( _outputId ) );
	}
	
	{
		TUTTLE_TCOUT( "---------------------------------------- Time Domain propagation" );
		graph::visitor::TimeDomain<InternalGraphImpl> timeDomainPropagationVisitor( _renderGraph );
		_renderGraph.depthFirstVisit( timeDomainPropagationVisitor, _renderGraph.getVertexDescriptor( _outputId ) );
	}
}

std::list<TimeRange> ProcessGraph::computeTimeRange()
{
	std::list<TimeRange> timeRanges = _options.getTimeRanges();

	TUTTLE_TCOUT_INFOS;
	if( timeRanges.empty() )
	{
		BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( _renderGraph.getVertexDescriptor(_outputId), _renderGraph.getGraph() ) )
		{
			TUTTLE_TCOUT_INFOS;
			ProcessVertex& v = _renderGraph.targetInstance( ed );
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
	return timeRanges;
}

void ProcessGraph::setupAtTime( const OfxTime time )
{
	//boost::timer::auto_cpu_timer timer;
	TUTTLE_TCOUT( "---------------------------------------- deploy time" );
	graph::visitor::DeployTime<InternalGraphImpl> deployTimeVisitor( _renderGraph, time );
	_renderGraph.depthFirstVisit( deployTimeVisitor, _renderGraph.getVertexDescriptor( _outputId ) );
#ifndef TUTTLE_PRODUCTION
	graph::exportDebugAsDOT( "graphProcess_c.dot", _renderGraph );
#endif

	TUTTLE_TCOUT( "---------------------------------------- build renderGraphAtTime" );
	// create a new graph with time information
	_renderGraphAtTime.clear();
	{
		BOOST_FOREACH( InternalGraphAtTimeImpl::vertex_descriptor vd, _renderGraph.getVertices() )
		{
			Vertex& v = _renderGraph.instance( vd );
			BOOST_FOREACH( const OfxTime t, v._data._times )
			{
				TUTTLE_TCOUT_VAR2( v, t );
				_renderGraphAtTime.addVertex( ProcessVertexAtTime(v, t) );
			}
		}
		BOOST_FOREACH( const InternalGraphAtTimeImpl::edge_descriptor ed, _renderGraph.getEdges() )
		{
			const Edge& e = _renderGraph.instance( ed );
			const Vertex& in = _renderGraph.sourceInstance( ed );
			const Vertex& out = _renderGraph.targetInstance( ed );
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

					_renderGraphAtTime.addEdge(
						_renderGraphAtTime.getVertexDescriptor( inKey ),
						_renderGraphAtTime.getVertexDescriptor( outKey ),
						eAtTime );
				}
			}
		}
	}

	InternalGraphAtTimeImpl::vertex_descriptor outputAtTime = getOutputVertexAtTime( time );
	
	// declare final nodes
	BOOST_FOREACH( const InternalGraphAtTimeImpl::edge_descriptor ed, boost::out_edges( outputAtTime, _renderGraphAtTime.getGraph() ) )
	{
		VertexAtTime& v = _renderGraphAtTime.targetInstance( ed );
		v.getProcessDataAtTime()._isFinalNode = true; /// @todo: this is maybe better to move this into the ProcessData? Doesn't depend on time?
	}

	TUTTLE_TCOUT( "---------------------------------------- set data at time" );
	// give a link to the node on its attached process data
	BOOST_FOREACH( const InternalGraphAtTimeImpl::vertex_descriptor vd, _renderGraphAtTime.getVertices() )
	{
		VertexAtTime& v = _renderGraphAtTime.instance(vd);
		if( ! v.isFake() )
		{
			//TUTTLE_TCOUT( "setProcessDataAtTime: " << v._name << " id: " << v._id << " at time: " << v._data._time );
			v.getProcessNode().setProcessDataAtTime( &v._data );
		}
	}

	bakeGraphInformationToNodes( _renderGraphAtTime );


#ifndef TUTTLE_PRODUCTION
	graph::exportDebugAsDOT( "graphProcessAtTime_a.dot", _renderGraphAtTime );
#endif

	if( ! _options.getForceIdentityNodesProcess() )
	{
		TUTTLE_TCOUT( "---------------------------------------- remove identity nodes" );
		// The "Remove identity nodes" step need to be done after preprocess steps, because the RoI need to be computed.
		std::vector<graph::visitor::IdentityNodeConnection<InternalGraphAtTimeImpl> > toRemove;

		graph::visitor::RemoveIdentityNodes<InternalGraphAtTimeImpl> vis( _renderGraphAtTime, toRemove );
		_renderGraphAtTime.depthFirstVisit( vis, outputAtTime );
		TUTTLE_TCOUT_VAR( toRemove.size() );
		if( toRemove.size() )
		{
			graph::visitor::removeIdentityNodes( _renderGraphAtTime, toRemove );

			// Bake graph information again as the connections have changed.
			bakeGraphInformationToNodes( _renderGraphAtTime );
		}
	}

#ifndef TUTTLE_PRODUCTION
	graph::exportDebugAsDOT( "graphProcessAtTime_b.dot", _renderGraphAtTime );
#endif

	{
		TUTTLE_TCOUT( "---------------------------------------- preprocess 1" );
		TUTTLE_TCOUT_INFOS;
		graph::visitor::PreProcess1<InternalGraphAtTimeImpl> preProcess1Visitor( _renderGraphAtTime );
		TUTTLE_TCOUT_INFOS;
		_renderGraphAtTime.depthFirstVisit( preProcess1Visitor, outputAtTime );
		TUTTLE_TCOUT_INFOS;
	}

	{
		TUTTLE_TCOUT( "---------------------------------------- preprocess 2" );
		graph::visitor::PreProcess2<InternalGraphAtTimeImpl> preProcess2Visitor( _renderGraphAtTime );
		_renderGraphAtTime.depthFirstVisit( preProcess2Visitor, outputAtTime );
	}

#ifndef TUTTLE_PRODUCTION
	graph::exportDebugAsDOT( "graphProcessAtTime_c.dot", _renderGraphAtTime );
#endif

	/*
	TUTTLE_TCOUT( "---------------------------------------- optimize graph" );
	graph::visitor::OptimizeGraph<InternalGraphAtTimeImpl> optimizeGraphVisitor( _renderGraphAtTime );
	_renderGraphAtTime.depthFirstVisit( optimizeGraphVisitor, outputAtTime );
	*/
#ifndef TUTTLE_PRODUCTION
	graph::exportDebugAsDOT( "graphProcessAtTime_d.dot", _renderGraphAtTime );
#endif
	/*
	InternalGraphImpl tmpGraph;
	output = _renderGraph.getVertexDescriptor( _outputId );
	/// @todo tuttle: out_edges sort don't work...
	TUTTLE_TCOUT( "---------------------------------------- sorting graph" );
	BOOST_FOREACH( InternalGraphImpl::vertex_descriptor vd, _renderGraph.getVertices() )
	{
		std::vector<InternalGraphImpl::Edge> edges( boost::out_degree(vd, _renderGraph.getGraph()) );

		BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( vd, _renderGraph.getGraph() ) )
		{
			edges.push_back( _renderGraph.instance(ed) );
		}

		Vertex& v = _renderGraph.instance(vd);
		TUTTLE_TCOUT_X( 30, "-" );
		std::size_t i = 0;
		TUTTLE_TCOUT( "before sort edges of " << v.getName() );
		BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( vd, _renderGraph.getGraph() ) )
		{
			Edge& e = _renderGraph.instance(ed);
			e._localId = i++;
			e._name += " -- ";
			e._name += boost::lexical_cast<std::string>(e._localId); // tmp
			TUTTLE_TCOUT( e.getName() << " - " <<  _renderGraph.targetInstance(ed).getProcessDataAtTime()._globalInfos._memory  );
		}
		std::sort( edges.begin(), edges.end(), SortEdgeByMemorySize<InternalGraphImpl>(_renderGraph) );
		TUTTLE_TCOUT( "after sort edges of " << v.getName() );
		BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( vd, _renderGraph.getGraph() ) )
		{
			Edge& e = _renderGraph.instance(ed);
			TUTTLE_TCOUT( e.getName() << " - " <<  _renderGraph.targetInstance(ed).getProcessDataAtTime()._globalInfos._memory );
		}
		InternalGraphImpl::out_edge_iterator oe_it, oe_itEnd;
		boost::tie( oe_it, oe_itEnd ) = boost::out_edges( vd, _renderGraph.getGraph() );
		for( ; oe_it != oe_itEnd; ++oe_it )
		{
			Edge& e = _renderGraph.instance(*oe_it);
			TUTTLE_TCOUT( e.getName() << " - " <<  _renderGraph.targetInstance(*oe_it).getProcessDataAtTime()._globalInfos._memory );
		}
	}
#ifndef TUTTLE_PRODUCTION
	graph::exportDebugAsDOT( "graphprocess_e.dot", tmpGraph );
#endif
	*/

}

void ProcessGraph::computeHashAtTime( NodeHashContainer& outNodesHash, const OfxTime time )
{
	//boost::timer::auto_cpu_timer timer;
	TUTTLE_TCOUT( "---------------------------------------- setupAtTime" );
	setupAtTime( time );
	TUTTLE_TCOUT( "---------------------------------------- computeHashAtTime BEGIN" );
	graph::visitor::ComputeHashAtTime<InternalGraphAtTimeImpl> computeHashAtTimeVisitor( _renderGraphAtTime, outNodesHash, time );
	InternalGraphAtTimeImpl::vertex_descriptor outputAtTime = getOutputVertexAtTime( time );
	_renderGraphAtTime.depthFirstVisit( computeHashAtTimeVisitor, outputAtTime );
	TUTTLE_TCOUT( "---------------------------------------- computeHashAtTime END" );
}

void ProcessGraph::processAtTime( memory::MemoryCache& outCache, const OfxTime time )
{
	//boost::timer::auto_cpu_timer timer;
	///@todo callback
	TUTTLE_TCOUT( tuttle::common::kColorBlue << "process at time " << time << tuttle::common::kColorStd );
	TUTTLE_TCOUT( "________________________________________ frame: " << time );

	TUTTLE_TCOUT( "________________________________________ output node : " << _renderGraph.getVertex( _outputId ).getName() );

	InternalGraphAtTimeImpl::vertex_descriptor outputAtTime = getOutputVertexAtTime( time );

	TUTTLE_TCOUT( "---------------------------------------- process" );
	// do the process
	graph::visitor::Process<InternalGraphAtTimeImpl> processVisitor( _renderGraphAtTime, core().getMemoryCache() );
	if( _options.getReturnBuffers() )
	{
		// accumulate output nodes buffers into the @p outCache MemoryCache
		processVisitor.setOutputMemoryCache( outCache );
	}

	_renderGraphAtTime.depthFirstVisit( processVisitor, outputAtTime );

	TUTTLE_TCOUT( "---------------------------------------- postprocess" );
	graph::visitor::PostProcess<InternalGraphAtTimeImpl> postProcessVisitor( _renderGraphAtTime );
	_renderGraphAtTime.depthFirstVisit( postProcessVisitor, outputAtTime );
/*
	///@todo clean datas...
	TUTTLE_TCOUT( "---------------------------------------- clear data at time" );
	// give a link to the node on its attached process data
	BOOST_FOREACH( const InternalGraphAtTimeImpl::vertex_descriptor vd, _renderGraphAtTime.getVertices() )
	{
		VertexAtTime& v = _renderGraphAtTime.instance(vd);
		if( ! v.isFake() )
		{
			v.getProcessNode().clearProcessDataAtTime();
		}
	}
*/
	// end of one frame
	// do some clean: memory clean, as temporary solution...
	TUTTLE_TCOUT( "---------------------------------------- clearUnused" );
	core().getMemoryCache().clearUnused();
	TUTTLE_TCOUT_VAR( core().getMemoryCache().size() );
	TUTTLE_TCOUT_VAR( core().getMemoryCache() );
	TUTTLE_TCOUT_VAR( outCache );

	TUTTLE_COUT( " " );
}

bool ProcessGraph::process( memory::MemoryCache& outCache )
{
#ifndef TUTTLE_PRODUCTION
	graph::exportAsDOT( "graphProcess_a.dot", _renderGraph );
#endif
	
	setup();
	
	TUTTLE_TCOUT_INFOS;
	std::list<TimeRange> timeRanges = computeTimeRange();

#ifndef TUTTLE_PRODUCTION
	graph::exportDebugAsDOT( "graphProcess_b.dot", _renderGraph );
#endif

	/// @todo Bug: need to use a map 'OutputNode': 'timeRanges'
	/// And check if all Output nodes share a common timeRange
	
	TUTTLE_TCOUT( "process render..." );
	//--- RENDER
	// at each frame
	
	
	ProcessVertexData procOptions;
	procOptions._interactive = _options.getIsInteractive();
	// imageEffect specific...
	procOptions._renderScale = _options.getRenderScale();
	
	BOOST_FOREACH( const TimeRange& timeRange, timeRanges )
	{
		TUTTLE_TCOUT( "timeRange: [" << timeRange._begin << ", " << timeRange._end << ", " << timeRange._step << "]" );
		procOptions._renderTimeRange.min = timeRange._begin;
		procOptions._renderTimeRange.max = timeRange._end;
		procOptions._step                = timeRange._step;

		beginSequenceRender( procOptions );
		
		for( int time = timeRange._begin; time <= timeRange._end; time += timeRange._step )
		{
			if( _options.getAbort() )
			{
				TUTTLE_COUT( tuttle::common::kColorRed << "PROCESS ABORTED at time " << time << "." << tuttle::common::kColorStd );
				endSequenceRender( procOptions );
				core().getMemoryCache().clearUnused();
				return false;
			}
			
			try
			{
				setupAtTime( time );

				processAtTime( outCache, time );
			}
			catch( tuttle::exception::FileInSequenceNotExist& e ) // @todo tuttle: change that.
			{
				if( _options.getContinueOnMissingFile() && ! _options.getAbort() )
				{
					TUTTLE_COUT( tuttle::common::kColorError << "Undefined input at time " << time << "." << tuttle::common::kColorStd << "\n" );
		#ifndef TUTTLE_PRODUCTION
					TUTTLE_COUT_ERROR( boost::diagnostic_information(e) );
		#endif
				}
				else
				{
					TUTTLE_TCOUT( "---------------------------------------- Error" );
					endSequenceRender( procOptions );
					core().getMemoryCache().clearUnused();
					throw;
				}
			}
			catch( ... )
			{
				if( _options.getContinueOnError() && ! _options.getAbort() )
				{
					TUTTLE_COUT( tuttle::common::kColorError << "Skip frame " << time << "." << tuttle::common::kColorStd );
		#ifndef TUTTLE_PRODUCTION
					TUTTLE_COUT_ERROR( "Skip frame " << time << "." );
					TUTTLE_COUT( boost::current_exception_diagnostic_information() );
		#endif
				}
				else
				{
					TUTTLE_TCOUT( "---------------------------------------- Error" );
					endSequenceRender( procOptions );
					core().getMemoryCache().clearUnused();
					throw;
				}
			}
		}
		
		endSequenceRender( procOptions );
	}
	return true;
}

}
}
}

