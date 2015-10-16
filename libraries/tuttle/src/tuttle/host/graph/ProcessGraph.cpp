#include "ProcessGraph.hpp"
#include "ProcessVisitors.hpp"
#include <tuttle/common/utils/color.hpp>
#include <tuttle/host/graph/GraphExporter.hpp>

#include <boost/foreach.hpp>

#if(TUTTLE_EXPORT_WITH_TIMER)
#include <boost/timer/timer.hpp>
#endif

namespace tuttle {
namespace host {
namespace graph {

const std::string ProcessGraph::_outputId( "TUTTLE_FAKE_OUTPUT" );

ProcessGraph::ProcessGraph( const ComputeOptions& options, Graph& userGraph, const std::list<std::string>& outputNodes, memory::IMemoryCache& internMemoryCache )
	: _instanceCount( userGraph.getInstanceCount() )
	, _options(options)
	, _internMemoryCache(internMemoryCache)
	, _procOptions(&_internMemoryCache)
{
	_procOptions._interactive = _options.getIsInteractive();
	// imageEffect specific...
	_procOptions._renderScale = _options.getRenderScale();
	
	updateGraph( userGraph, outputNodes );
}

ProcessGraph::~ProcessGraph()
{}


ProcessGraph::VertexAtTime::Key ProcessGraph::getOutputKeyAtTime( const OfxTime time )
{
	return VertexAtTime(Vertex(_procOptions, _outputId), time).getKey();
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
			tuttle::host::INode& origNode = v.getProcessNode(); // pointer of the copied graph, we don't own it !
#else
			const tuttle::host::INode& origNode = v.getProcessNode(); // pointer of the copied graph, we don't own it !
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

   //cout << "A topological ordering: ";
   //for( container::reverse_iterator ii=c.rbegin(); ii!=c.rend(); ++ii )
   //cout << index(*ii) << " ";
   //cout << endl;
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
//		TUTTLE_LOG_VAR2(v1.getName(), v1.getProcessDataAtTime()._globalInfos._memory);
//		TUTTLE_LOG_VAR2(v2.getName(), v2.getProcessDataAtTime()._globalInfos._memory);
//		TUTTLE_LOG_VAR(res);
		return res;
	}
private:
	const TGraph& _renderGraph;
};
*/

void ProcessGraph::bakeGraphInformationToNodes( InternalGraphAtTimeImpl& _renderGraphAtTime )
{
	BOOST_FOREACH( const InternalGraphAtTimeImpl::vertex_descriptor vd, _renderGraphAtTime.getVertices() )
	{
		VertexAtTime& v = _renderGraphAtTime.instance( vd );
		ProcessVertexAtTimeData& vData = v.getProcessDataAtTime();
		
		TUTTLE_LOG_INFO( "[bake graph information to nodes] node: " << v.getName() );

		vData._outDegree = _renderGraphAtTime.getInDegree( vd );
		vData._inDegree = _renderGraphAtTime.getOutDegree( vd );

		vData._outEdges.clear();
		vData._outEdges.reserve( vData._outDegree );
		BOOST_FOREACH( const InternalGraphAtTimeImpl::edge_descriptor ed, _renderGraphAtTime.getInEdges( vd ) )
		{
			const ProcessEdgeAtTime* e = &_renderGraphAtTime.instance(ed);

			TUTTLE_LOG_INFO( "[bake graph information to nodes] in edge " << e->getInAttrName() << ", at time " << e->getInTime() );
			vData._outEdges.push_back( e );
		}
		vData._inEdges.clear();
		BOOST_FOREACH( const InternalGraphAtTimeImpl::edge_descriptor ed, _renderGraphAtTime.getOutEdges( vd ) )
		{
			const ProcessEdgeAtTime* e = &_renderGraphAtTime.instance(ed);
			TUTTLE_LOG_INFO( "[bake graph information to nodes] out edge " << e->getInAttrName() << ", at time " << e->getInTime() );
			std::pair<std::string, OfxTime> key( e->getInAttrName(), e->getInTime() );
			vData._inEdges[key] = e;
		}
	}
	TUTTLE_LOG_INFO( "[bake graph information to nodes] connect clips" );
	connectClips<InternalGraphAtTimeImpl>( _renderGraphAtTime );

}

void ProcessGraph::beginSequence( const TimeRange& timeRange )
{
	_options.beginSequenceHandle();
	_procOptions._renderTimeRange.min = timeRange._begin;
	_procOptions._renderTimeRange.max = timeRange._end;
	_procOptions._step                = timeRange._step;

	TUTTLE_LOG_INFO( "[begin sequence] start" );
	//	BOOST_FOREACH( NodeMap::value_type& p, _nodes )
	for( NodeMap::iterator it = _nodes.begin(), itEnd = _nodes.end();
		it != itEnd;
		++it )
	{
		NodeMap::value_type& p = *it;
		p.second->beginSequence( _procOptions );
	}
}

void ProcessGraph::endSequence()
{
	_options.endSequenceHandle();
	TUTTLE_LOG_INFO( "[Process render] process end sequence" );
	//--- END sequence render
	BOOST_FOREACH( NodeMap::value_type& p, _nodes )
	{
		p.second->endSequence( _procOptions ); // node option... or no option here ?
	}
}

void ProcessGraph::updateGraph( Graph& userGraph, const std::list<std::string>& outputNodes )
{
	_renderGraph.copyTransposed( userGraph.getGraph() );

	Vertex outputVertex( _procOptions, _outputId );

	if( outputNodes.size() )
	{
		_renderGraph.addVertex( outputVertex );
		BOOST_FOREACH( const std::string & s, outputNodes )
		{
			_renderGraph.connect( _outputId, s, "Output" );
			TUTTLE_LOG_DEBUG( "MY OUTPUT: " << s );
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
	TUTTLE_LOG_INFO( "[Process render] setup" );
	
	// Initialize variables
//	OfxRectD renderWindow = { 0, 0, 0, 0 };

	//--- BEGIN RENDER

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
			v.setProcessData( _procOptions );
			v.getProcessNode().setProcessData( &v._data );
		}
	}
	
	connectClips<InternalGraphImpl>( _renderGraph );
	
	{
		TUTTLE_LOG_INFO( "[Process render] Time domain propagation" );
		graph::visitor::TimeDomain<InternalGraphImpl> timeDomainPropagationVisitor( _renderGraph );
		_renderGraph.depthFirstVisit( timeDomainPropagationVisitor, _renderGraph.getVertexDescriptor( _outputId ) );
	}

	{	
		TUTTLE_LOG_INFO( "[Process render] setup visitors" );
		graph::visitor::Setup1<InternalGraphImpl> setup1Visitor( _renderGraph );
		_renderGraph.depthFirstVisit( setup1Visitor, _renderGraph.getVertexDescriptor( _outputId ) );
		graph::visitor::Setup2<InternalGraphImpl> setup2Visitor( _renderGraph );
		_renderGraph.depthFirstVisit( setup2Visitor, _renderGraph.getVertexDescriptor( _outputId ) );
		graph::visitor::Setup3<InternalGraphImpl> setup3Visitor( _renderGraph );
		_renderGraph.depthFirstVisit( setup3Visitor, _renderGraph.getVertexDescriptor( _outputId ) );
	}
}

std::list<TimeRange> ProcessGraph::computeTimeRange()
{
	std::list<TimeRange> timeRanges = _options.getTimeRanges();

	TUTTLE_LOG_INFOS;
	if( timeRanges.empty() )
	{
		BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( _renderGraph.getVertexDescriptor(_outputId), _renderGraph.getGraph() ) )
		{
			//TUTTLE_LOG_INFOS;
			ProcessVertex& v = _renderGraph.targetInstance( ed );
			// compute the time domain for each output node
			//TUTTLE_LOG_INFOS;
			OfxRangeD timeDomain = v.getProcessData()._timeDomain;
			TUTTLE_LOG_VAR2( TUTTLE_TRACE, timeDomain.min, timeDomain.max );
			
			if( _options.getBegin() != std::numeric_limits<int>::min() && timeDomain.min < _options.getBegin() )
				timeDomain.min = _options.getBegin();
			if( _options.getEnd() != std::numeric_limits<int>::max() && timeDomain.max > _options.getEnd() )
				timeDomain.max = _options.getEnd();
			
			TUTTLE_LOG_VAR2( TUTTLE_TRACE, timeDomain.min, timeDomain.max );
			// special case for infinite time domain (eg. a still image)
			if( timeDomain.min <= kOfxFlagInfiniteMin )
				timeDomain.min = 0;
			if( timeDomain.max >= kOfxFlagInfiniteMax )
				timeDomain.max = 0;

			//TUTTLE_LOG_INFOS;
			timeRanges.push_back( TimeRange( timeDomain ) );
			TUTTLE_LOG_INFOS;
			TUTTLE_LOG_INFO( "Compute " << quotes(v.getName()) << " full time domain: from " << timeDomain.min << " to " << timeDomain.max << "." );
		}
	}
	return timeRanges;
}

void ProcessGraph::setupAtTime( const OfxTime time )
{
	_options.setupAtTimeHandle();
#if(TUTTLE_EXPORT_WITH_TIMER)
	boost::timer::cpu_timer timer;
#endif
	
	TUTTLE_LOG_TRACE( "[Setup at time " << time << "] start" );
	graph::visitor::DeployTime<InternalGraphImpl> deployTimeVisitor( _renderGraph, time );
	_renderGraph.depthFirstVisit( deployTimeVisitor, _renderGraph.getVertexDescriptor( _outputId ) );
#if(TUTTLE_EXPORT_PROCESSGRAPH_DOT)
	graph::exportDebugAsDOT( "graphProcess_c.dot", _renderGraph );
#endif

	TUTTLE_LOG_TRACE( "[Setup at time " << time << "] build render graph" );
	// create a new graph with time information
	_renderGraphAtTime.clear();
	
	{
		BOOST_FOREACH( InternalGraphAtTimeImpl::vertex_descriptor vd, _renderGraph.getVertices() )
		{
			Vertex& v = _renderGraph.instance( vd );
			BOOST_FOREACH( const OfxTime t, v._data._times )
			{
				TUTTLE_LOG_INFO( "[Setup at time " << time << "] add connection from node: " << v << " for time: " << t );
				_renderGraphAtTime.addVertex( ProcessVertexAtTime(v, t) );
			}
		}
		BOOST_FOREACH( const InternalGraphAtTimeImpl::edge_descriptor ed, _renderGraph.getEdges() )
		{
			const Edge& e = _renderGraph.instance( ed );
			const Vertex& in = _renderGraph.sourceInstance( ed );
			const Vertex& out = _renderGraph.targetInstance( ed );
			TUTTLE_LOG_INFO( "[Setup at time " << time << "] set connection " << e );
			BOOST_FOREACH( const Edge::TimeMap::value_type& tm, e._timesNeeded )
			{
				const VertexAtTime procIn( in, tm.first );
				BOOST_FOREACH( const OfxTime t2, tm.second )
				{
					//TUTTLE_LOG_VAR( TUTTLE_TRACE, tm.first );
					//TUTTLE_LOG_VAR( TUTTLE_TRACE, t2 );
					const VertexAtTime procOut( out, t2 );

					const VertexAtTime::Key inKey( procIn.getKey() );
					const VertexAtTime::Key outKey( procOut.getKey() );

					//TUTTLE_LOG_VAR( TUTTLE_TRACE, inKey );
					//TUTTLE_LOG_VAR( TUTTLE_TRACE, outKey );
					//TUTTLE_LOG_VAR( TUTTLE_TRACE, e.getInAttrName() );

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

	TUTTLE_LOG_INFO( "[Setup at time " << time << "] set data at time" );
	// give a link to the node on its attached process data
	BOOST_FOREACH( const InternalGraphAtTimeImpl::vertex_descriptor vd, _renderGraphAtTime.getVertices() )
	{
		VertexAtTime& v = _renderGraphAtTime.instance(vd);
		if( ! v.isFake() )
		{
			//TUTTLE_LOG_INFO( "setProcessDataAtTime: " << v._name << " id: " << v._id << " at time: " << v._data._time );
			v.getProcessNode().setProcessDataAtTime( &v._data );
		}
	}

	bakeGraphInformationToNodes( _renderGraphAtTime );


#if(TUTTLE_EXPORT_PROCESSGRAPH_DOT)
	graph::exportDebugAsDOT( "graphProcessAtTime_a.dot", _renderGraphAtTime );
#endif

	if( ! _options.getForceIdentityNodesProcess() )
	{
		TUTTLE_LOG_TRACE( "[Setup at time " << time << "] remove identity nodes" );
		// The "Remove identity nodes" step need to be done after preprocess steps, because the RoI need to be computed.
		std::vector<graph::visitor::IdentityNodeConnection<InternalGraphAtTimeImpl> > toRemove;

		graph::visitor::RemoveIdentityNodes<InternalGraphAtTimeImpl> vis( _renderGraphAtTime, toRemove );
		_renderGraphAtTime.depthFirstVisit( vis, outputAtTime );
		TUTTLE_LOG_TRACE( "[Setup at time " << time << "] removing " << toRemove.size() << " nodes" );
		if( toRemove.size() )
		{
			graph::visitor::removeIdentityNodes( _renderGraphAtTime, toRemove );

			// Bake graph information again as the connections have changed.
			bakeGraphInformationToNodes( _renderGraphAtTime );
		}
	}

#if(TUTTLE_EXPORT_PROCESSGRAPH_DOT)
	graph::exportDebugAsDOT( "graphProcessAtTime_b.dot", _renderGraphAtTime );
#endif

	{
		TUTTLE_LOG_TRACE( "[Setup at time " << time << "] preprocess 1" );
		graph::visitor::PreProcess1<InternalGraphAtTimeImpl> preProcess1Visitor( _renderGraphAtTime );
		_renderGraphAtTime.depthFirstVisit( preProcess1Visitor, outputAtTime );
	}

	{
		TUTTLE_LOG_TRACE( "[Setup at time " << time << "] preprocess 2" );
		graph::visitor::PreProcess2<InternalGraphAtTimeImpl> preProcess2Visitor( _renderGraphAtTime );
		_renderGraphAtTime.depthFirstVisit( preProcess2Visitor, outputAtTime );
	}

#if(TUTTLE_EXPORT_PROCESSGRAPH_DOT)
	graph::exportDebugAsDOT( "graphProcessAtTime_c.dot", _renderGraphAtTime );
#endif

	/*
	TUTTLE_LOG_INFO( "---------------------------------------- optimize graph" );
	graph::visitor::OptimizeGraph<InternalGraphAtTimeImpl> optimizeGraphVisitor( _renderGraphAtTime );
	_renderGraphAtTime.depthFirstVisit( optimizeGraphVisitor, outputAtTime );
	*/
#if(TUTTLE_EXPORT_PROCESSGRAPH_DOT)
	graph::exportDebugAsDOT( "graphProcessAtTime_d.dot", _renderGraphAtTime );
#endif
	/*
	InternalGraphImpl tmpGraph;
	output = _renderGraph.getVertexDescriptor( _outputId );
	/// @todo tuttle: out_edges sort don't work...
	TUTTLE_LOG_INFO( "---------------------------------------- sorting graph" );
	BOOST_FOREACH( InternalGraphImpl::vertex_descriptor vd, _renderGraph.getVertices() )
	{
		std::vector<InternalGraphImpl::Edge> edges( boost::out_degree(vd, _renderGraph.getGraph()) );

		BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( vd, _renderGraph.getGraph() ) )
		{
			edges.push_back( _renderGraph.instance(ed) );
		}

		Vertex& v = _renderGraph.instance(vd);

		std::size_t i = 0;
		TUTTLE_LOG_INFO( "before sort edges of " << v.getName() );
		BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( vd, _renderGraph.getGraph() ) )
		{
			Edge& e = _renderGraph.instance(ed);
			e._localId = i++;
			e._name += " -- ";
			e._name += boost::lexical_cast<std::string>(e._localId); // tmp
			TUTTLE_LOG_INFO( e.getName() << " - " <<  _renderGraph.targetInstance(ed).getProcessDataAtTime()._globalInfos._memory  );
		}
		std::sort( edges.begin(), edges.end(), SortEdgeByMemorySize<InternalGraphImpl>(_renderGraph) );
		TUTTLE_LOG_INFO( "after sort edges of " << v.getName() );
		BOOST_FOREACH( InternalGraphImpl::edge_descriptor ed, boost::out_edges( vd, _renderGraph.getGraph() ) )
		{
			Edge& e = _renderGraph.instance(ed);
			TUTTLE_LOG_INFO( e.getName() << " - " <<  _renderGraph.targetInstance(ed).getProcessDataAtTime()._globalInfos._memory );
		}
		InternalGraphImpl::out_edge_iterator oe_it, oe_itEnd;
		boost::tie( oe_it, oe_itEnd ) = boost::out_edges( vd, _renderGraph.getGraph() );
		for( ; oe_it != oe_itEnd; ++oe_it )
		{
			Edge& e = _renderGraph.instance(*oe_it);
			TUTTLE_LOG_INFO( e.getName() << " - " <<  _renderGraph.targetInstance(*oe_it).getProcessDataAtTime()._globalInfos._memory );
		}
	}
#if(TUTTLE_EXPORT_PROCESSGRAPH_DOT)
	graph::exportDebugAsDOT( "graphprocess_e.dot", tmpGraph );
#endif
	*/

}

void ProcessGraph::computeHashAtTime( NodeHashContainer& outNodesHash, const OfxTime time )
{
#if(TUTTLE_EXPORT_WITH_TIMER)
	boost::timer::cpu_timer timer;
#endif
	setupAtTime( time );
	TUTTLE_LOG_INFO( "[Compute hash at time] begin" );
	graph::visitor::ComputeHashAtTime<InternalGraphAtTimeImpl> computeHashAtTimeVisitor( _renderGraphAtTime, outNodesHash, time );
	InternalGraphAtTimeImpl::vertex_descriptor outputAtTime = getOutputVertexAtTime( time );
	_renderGraphAtTime.depthFirstVisit( computeHashAtTimeVisitor, outputAtTime );
	TUTTLE_LOG_INFO( "[Compute hash at time] end" );
}

void ProcessGraph::processAtTime( memory::IMemoryCache& outCache, const OfxTime time )
{
	_options.processAtTimeHandle();
#if(TUTTLE_EXPORT_WITH_TIMER)
	boost::timer::cpu_timer timer;
#endif
	
	TUTTLE_LOG_TRACE( "[Process at time " << time << "] Output node : " << _renderGraph.getVertex( _outputId ).getName() );
	InternalGraphAtTimeImpl::vertex_descriptor outputAtTime = getOutputVertexAtTime( time );

    // Launch a pass of callbacks on the nodes
    graph::visitor::BeforeRenderCallbackVisitor<InternalGraphAtTimeImpl> 
        callbackRun( _renderGraphAtTime );
    _renderGraphAtTime.depthFirstVisit( callbackRun, outputAtTime );

	// do the process
	graph::visitor::Process<InternalGraphAtTimeImpl> processVisitor( _renderGraphAtTime, _internMemoryCache );
	if( _options.getReturnBuffers() )
	{
		// accumulate output nodes buffers into the @p outCache MemoryCache
		processVisitor.setOutputMemoryCache( outCache );
	}

	_renderGraphAtTime.depthFirstVisit( processVisitor, outputAtTime );

	TUTTLE_LOG_TRACE( "[Process at time " << time << "] Post process" );
	graph::visitor::PostProcess<InternalGraphAtTimeImpl> postProcessVisitor( _renderGraphAtTime );
	_renderGraphAtTime.depthFirstVisit( postProcessVisitor, outputAtTime );

	///@todo clean datas...
	TUTTLE_LOG_TRACE( "[Process at time " << time << "] Clear data at time" );
	// give a link to the node on its attached process data
	BOOST_FOREACH( const InternalGraphAtTimeImpl::vertex_descriptor vd, _renderGraphAtTime.getVertices() )
	{
		VertexAtTime& v = _renderGraphAtTime.instance(vd);
		if( ! v.isFake() )
		{
			v.getProcessNode().clearProcessDataAtTime();
		}
	}

	// clear cache at each frame
	// @todo: remove
	_internMemoryCache.clearUnused();

	TUTTLE_LOG_TRACE( "[Process at time " << time << "] Memory cache size: " << _internMemoryCache.size() );
	TUTTLE_LOG_TRACE( "[Process at time " << time << "] Out cache size: " << outCache.size() );
}

bool ProcessGraph::process( memory::IMemoryCache& outCache )
{
#if(TUTTLE_EXPORT_WITH_TIMER)
	boost::timer::cpu_timer all_process_timer;
#endif

#if(TUTTLE_EXPORT_PROCESSGRAPH_DOT)
	graph::exportAsDOT( "graphProcess_a.dot", _renderGraph );
#endif

	setup();

	std::list<TimeRange> timeRanges = computeTimeRange();

#if(TUTTLE_EXPORT_PROCESSGRAPH_DOT)
	graph::exportDebugAsDOT( "graphProcess_b.dot", _renderGraph );
#endif

	/// @todo Bug: need to use a map 'OutputNode': 'timeRanges'
	/// And check if all Output nodes share a common timeRange

	TUTTLE_LOG_INFO( "[Process render] start" );

	// Begin range of frames
	TimeRange globalTimeRange( timeRanges.back() );
	BOOST_FOREACH( const TimeRange& range, timeRanges )
	{
		if( globalTimeRange._begin > range._begin )
			globalTimeRange._begin = range._begin;
		if( globalTimeRange._end < range._end )
			globalTimeRange._end = range._end;
	}
	TUTTLE_LOG_TRACE( "[Process render] begin timeRange: [" << globalTimeRange._begin << ", " << globalTimeRange._end << "]" );
	beginSequence( globalTimeRange );

	// RENDER (at each frame)
	BOOST_FOREACH( const TimeRange& timeRange, timeRanges )
	{
		TUTTLE_LOG_TRACE( "[Process render] process timeRange: [" << timeRange._begin << ", " << timeRange._end << ", " << timeRange._step << "]" );

		// If someone had asked to abort the process
		if( _options.getAbort() )
		{
			TUTTLE_LOG_ERROR( "[Process render] PROCESS ABORTED before first frame." );
			endSequence();
			_internMemoryCache.clearUnused();
			return false;
		}

		for( int time = timeRange._begin; time <= timeRange._end; time += timeRange._step )
		{
			_options.beginFrameHandle();

			try
			{
#if(TUTTLE_EXPORT_WITH_TIMER)
				boost::timer::cpu_timer setup_timer;
#endif
				setupAtTime( time );
#if(TUTTLE_EXPORT_WITH_TIMER)
				TUTTLE_LOG_INFO( "[process timer] setup " << boost::timer::format(setup_timer.elapsed()) );
#endif

#if(TUTTLE_EXPORT_WITH_TIMER)
				boost::timer::cpu_timer processAtTime_timer;
#endif
				processAtTime( outCache, time );
#if(TUTTLE_EXPORT_WITH_TIMER)
				TUTTLE_LOG_INFO( "[process timer] took " << boost::timer::format(processAtTime_timer.elapsed()) );
#endif
			}
			catch( tuttle::exception::FileInSequenceNotExist& e ) // @todo tuttle: change that.
			{
				e << tuttle::exception::time(time);
				if( _options.getContinueOnError() || _options.getContinueOnMissingFile() )
				{
					TUTTLE_LOG_WARNING( "[Process render] Missing input file at frame " << time << "." << std::endl
							<< tuttle::exception::format_exception_message(e) << std::endl
							<< tuttle::exception::format_exception_info(e)
						);
				}
				else
				{
					TUTTLE_LOG_ERROR( "[Process render] Missing input file at frame " << time << "." << std::endl );
					_options.endFrameHandle();
					endSequence();
					_renderGraphAtTime.clear();
					_internMemoryCache.clearUnused();
					throw;
				}
			}
			catch( ::boost::exception& e )
			{
				e << tuttle::exception::time(time);
				if( _options.getContinueOnError() )
				{
					TUTTLE_LOG_ERROR( "[Process render] Skip frame " << time << "." << std::endl
							<< tuttle::exception::format_exception_message(e) << std::endl
							<< tuttle::exception::format_exception_info(e)
						);
				}
				else
				{
					TUTTLE_LOG_ERROR( "[Process render] Stopped at frame " << time << "." << std::endl );
					_options.endFrameHandle();
					endSequence();
					_renderGraphAtTime.clear();
					_internMemoryCache.clearUnused();
					throw;
				}
			}
			catch(...)
			{
				if( _options.getContinueOnError() )
				{
					TUTTLE_LOG_ERROR( "[Process render] Skip frame " << time << "." << std::endl
							<< tuttle::exception::format_current_exception()
						);
				}
				else
				{
					TUTTLE_LOG_ERROR( "[Process render] Error at frame " << time << "." << std::endl );
					_options.endFrameHandle();
					endSequence();
					_renderGraphAtTime.clear();
					_internMemoryCache.clearUnused();
					throw;
				}
			}

			if( _options.getAbort() )
			{
				TUTTLE_LOG_ERROR( "[Process render] PROCESS ABORTED at time " << time << "." );
				_options.endFrameHandle();
				endSequence();
				_renderGraphAtTime.clear();
				_internMemoryCache.clearUnused();
				return false;
			}
			_options.endFrameHandle();
		}
	}

	// End range of frames
	endSequence();

#if(TUTTLE_EXPORT_WITH_TIMER)
	TUTTLE_LOG_INFO( "[all process timer] " << boost::timer::format(all_process_timer.elapsed()) );
#endif
	return true;
}

}
}
}

