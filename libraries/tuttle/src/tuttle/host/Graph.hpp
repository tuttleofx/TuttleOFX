#ifndef _TUTTLE_HOST_CORE_GRAPH_HPP_
#define _TUTTLE_HOST_CORE_GRAPH_HPP_

#include "NodeListArg.hpp"
#include "ComputeOptions.hpp"
#include "Core.hpp"
#include "INode.hpp"
#include "InputBufferWrapper.hpp"
#include "OutputBufferWrapper.hpp"
#include "exceptions.hpp"

#include <tuttle/host/graph/InternalGraph.hpp>
#include <tuttle/host/graph/UVertex.hpp>
#include <tuttle/host/graph/UEdge.hpp>
#include <tuttle/host/NodeAtTimeKey.hpp>
#include <tuttle/host/NodeHashContainer.hpp>
#include <tuttle/host/attribute/Attribute.hpp>
#include <tuttle/host/memory/MemoryCache.hpp>
#include <tuttle/common/utils/global.hpp>

#include <boost/ptr_container/ptr_map.hpp>

#include <stdexcept>
#include <string>
#include <sstream>
#include <map>
#include <list>

namespace tuttle
{
namespace host
{

class NodeInit;

/**
 * @brief A user graph to manipulate OpenFX nodes.
 */
class Graph
{
public:
    typedef graph::UVertex Vertex;
    typedef graph::UEdge Edge;
    typedef INode Node; /// @todo tuttle INode...
    typedef attribute::Attribute Attribute;
    typedef graph::InternalGraph<Vertex, Edge> InternalGraphImpl;
    typedef InternalGraphImpl::vertex_descriptor vertex_descriptor;
    typedef InternalGraphImpl::edge_descriptor edge_descriptor;

    typedef std::map<std::string, int> InstanceCountMap;
    typedef boost::ptr_map<std::string, Node> NodeMap;

public:
    Graph();
    // Graph( const Graph& other );
    ~Graph();

    /**
     * @brief Create a new input node in the current graph,
     *        to give an input buffer.
     */
    InputBufferWrapper createInputBuffer();

    /**
     * @brief Create a new output buffer node in the current graph,
              wrapped up for easy use
     */
    OutputBufferWrapper createOutputBuffer();

    /**
     * @brief Create a new node in the graph.
     * @param id is the plugin unique string identification (e.g. "tuttle.blur").
     */
    Node& createNode(const std::string& id);

    /**
     * @brief Add a node to the graph.
     *        It takes the ownership of the node object.
     *
     * @warning: The node will be renamed.
     */
    Node& addNode(const NodeInit& node);

    /**
     * @brief Add a node to the graph.
     *        It takes the ownership of the node object.
     *
     * @warning: The node will be renamed.
     */
    Node& addNode(INode& node);

    /**
     * @brief Add nodes to the graph.
     *
     * @warning: Nodes will be renamed.
     */
    std::vector<INode*> addNodes(const std::vector<NodeInit>& nodes);

    /**
     * @brief Add nodes to the graph and connect them linearly.
     *
     * @warning: Nodes will be renamed.
     */
    std::vector<INode*> addConnectedNodes(const std::vector<NodeInit>& nodes);

    /**
     * @brief Rename a node in the current graph.
     * @param newUniqueName is the new unique node name.
     *
     * @warning you will loose all connections.
     */
    void renameNode(Node& node, const std::string& newUniqueName);

    /**
     * @brief Delete a node from the current graph.
     * This will remove all the connections.
     */
    void deleteNode(Node& node);

    /**
     * @brief Delete all unconnected nodes from the current graph.
     * @param node: delete all unconnected nodes from this node.
     * @return the number of nodes deleted.
     */
    std::size_t deleteUnconnectedNodes(const Node& node);

    /**
     * @brief Delete all nodes from the current graph.
     * This will remove all the connections.
     */
    void clear();

    /**
     * @brief Connect nodes (using there unique name in this graph).
     */
    void connect(const std::string& outNode, const std::string& inNode,
                 const std::string& inAttr = kOfxSimpleSourceAttributeName);
    /**
     * @brief Connect nodes the list of nodes linearly.
     */
    void connect(const std::list<std::string>& nodes);
    void connect(const Node& outNode, const Node& inNode);
    void connect(const std::list<Node*>& nodes);
    void connect(const std::vector<Node*>& nodes);
    void connect(const Node& outNode, const Attribute& inAttr);
    void connect(const Attribute& outAttr, const Attribute& inAttr);
    void unconnect(const Attribute& outAttr, const Attribute& inAttr);

    void unconnect(const Node& node);

    void replaceNodeConnections(const Node& fromNode, const Node& toNode);

    std::size_t getNbInputConnections(const Node& node) const;
    std::size_t getNbOutputConnections(const Node& node) const;

    /**
     * @brief Temporary solution ! Prepare the user graph, so we can call getTimeDomain (and maybe others functions) on
     * nodes.
     */
    void init();

    void setup();

    void setupAtTime(const OfxTime time, const NodeListArg& nodes = NodeListArg());

    void computeGlobalHashAtTime(NodeHashContainer& outNodesHash, const OfxTime time,
                                 const NodeListArg& nodes = NodeListArg());

    bool compute(const ComputeOptions& options = ComputeOptions());

    bool compute(const NodeListArg& nodes, const ComputeOptions& options = ComputeOptions());

    bool compute(memory::IMemoryCache& memoryCache, const ComputeOptions& options);

    bool compute(memory::IMemoryCache& memoryCache, const NodeListArg& nodes = NodeListArg(),
                 const ComputeOptions& options = ComputeOptions());

    bool compute(memory::IMemoryCache& memoryCache, const NodeListArg& nodes, const ComputeOptions& options,
                 memory::IMemoryCache& internMemoryCache);

public:
    inline const InternalGraphImpl& getGraph() const { return _graph; }

    inline const NodeMap& getNodesMap() const { return _nodesMap; }
    inline NodeMap& getNodesMap() { return _nodesMap; }

    std::vector<const Node*> getNodes() const;
    std::vector<Node*> getNodes();

    std::vector<Node*> getConnectedNodes(const Node& node);
    std::vector<Node*> getUnconnectedNodes(const Node& node);

    inline std::size_t getNbNodes() const { return _nodesMap.size(); }
    inline std::size_t getNbConnections() const { return _graph.getEdgeCount(); }

    std::vector<Node*> getNodesByContext(const std::string& type);
    std::vector<Node*> getNodesByPlugin(const std::string& pluginId);

    //	const Node& getNode( const std::string& name ) const { return getNodesMap()[name]; }
    inline const Node& getNode(const std::string& name) const { return _nodesMap.at(name); }
    inline Node& getNode(const std::string& name) { return getNodesMap().at(name); }

    inline const InstanceCountMap& getInstanceCount() const { return _instanceCount; }

public:
    enum EDotExportLevel
    {
        eDotExportLevelSimple,
        eDotExportLevelDetailed
    };
    void exportDot(const std::string& filename, const EDotExportLevel level = eDotExportLevelSimple) const;

    friend std::ostream& operator<<(std::ostream& os, const Graph& g);

private:
    InternalGraphImpl _graph;
    NodeMap _nodesMap;
    InstanceCountMap _instanceCount; ///< used to assign a unique name to each node

private:
    void addToInternalGraph(Node& node);
    void removeFromInternalGraph(Node& node);
};
}
}

#endif
