#ifndef _TUTTLE_HOST_PROCESSGRAPH_HPP_
#define _TUTTLE_HOST_PROCESSGRAPH_HPP_

#include "ProcessVertex.hpp"
#include "ProcessVertexAtTime.hpp"
#include "ProcessEdge.hpp"
#include "ProcessEdgeAtTime.hpp"

#include "InternalGraph.hpp"

#include <tuttle/host/Graph.hpp>
#include <tuttle/host/NodeHashContainer.hpp>

#include <string>

/**
 * @brief If there is a define PROCESSGRAPH_USE_LINK, we don't create a copy of all nodes and
 * The ProcessGraph only contains link to the node in the original Graph.
 */
#define PROCESSGRAPH_USE_LINK

namespace tuttle
{
namespace host
{
namespace graph
{

/**
 * @brief Created from a user Graph, this class allows you to launch the process.
 *
 * Internally this class use multiple graphs with different representation of the graph.
 * It create a new graph with a vertex for each node at each time.
 * It reorder the nodes to optimise memory usage.
 */
class ProcessGraph
{
public:
    typedef Graph::Node Node; /// @todo tuttle ProcessNode...
    typedef graph::ProcessVertex Vertex;
    typedef graph::ProcessVertexAtTime VertexAtTime;
    typedef graph::ProcessEdge Edge;
    typedef graph::ProcessEdgeAtTime EdgeAtTime;
    typedef Graph::Attribute Attribute;
    typedef InternalGraph<Vertex, Edge> InternalGraphImpl;
    typedef InternalGraph<ProcessVertexAtTime, ProcessEdgeAtTime> InternalGraphAtTimeImpl;
#ifdef PROCESSGRAPH_USE_LINK
    typedef std::map<std::string, Node*> NodeMap;
#else
    typedef Graph::NodeMap NodeMap;
#endif
    typedef Graph::InstanceCountMap InstanceCountMap;

public:
    ProcessGraph(const ComputeOptions& options, Graph& graph, const std::list<std::string>& nodes,
                 memory::IMemoryCache& internMemoryCache); ///@ todo: const Graph, no ?
    ~ProcessGraph();

private:
    VertexAtTime::Key getOutputKeyAtTime(const OfxTime time);
    InternalGraphAtTimeImpl::vertex_descriptor getOutputVertexAtTime(const OfxTime time);

    void relink();
    void bakeGraphInformationToNodes(InternalGraphAtTimeImpl& renderGraphAtTime);

public:
    void updateGraph(Graph& userGraph, const std::list<std::string>& outputNodes);

    void setup();
    std::list<TimeRange> computeTimeRange();
    void computeHashAtTime(NodeHashContainer& outNodesHash, const OfxTime time);

    void beginSequence(const TimeRange& timeRange);
    void setupAtTime(const OfxTime time);
    void processAtTime(memory::IMemoryCache& outCache, const OfxTime time);
    void endSequence();

    bool process(memory::IMemoryCache& outCache);

private:
    InternalGraphImpl _renderGraph;
    InternalGraphAtTimeImpl _renderGraphAtTime;
    NodeMap _nodes;
    InstanceCountMap _instanceCount;

    static const std::string _outputId;

    const ComputeOptions& _options;
    memory::IMemoryCache& _internMemoryCache;
    ProcessVertexData _procOptions;
};
}
}
}

#endif
