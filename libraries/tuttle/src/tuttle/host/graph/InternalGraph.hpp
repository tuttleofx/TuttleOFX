#ifndef _TUTTLE_HOST_INTERNALGRAPH_HPP_
#define _TUTTLE_HOST_INTERNALGRAPH_HPP_

#include <tuttle/host/exceptions.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/copy.hpp>
#include <boost/graph/transpose_graph.hpp>
#include <boost/graph/dominator_tree.hpp>
#include <boost/foreach.hpp>
#include <boost/exception/all.hpp>
#include <boost/unordered_map.hpp>

#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>

// include this file after the definition of basic boost::graph properties
#include <tuttle/host/graph/Visitors.hpp>

namespace tuttle
{
namespace host
{
namespace graph
{

namespace detail
{

template <class GraphIn, class GraphOut>
class Copier
{
public:
    Copier(const GraphIn& gIn, GraphOut& gOut)
        : _gIn(gIn)
        , _gOut(gOut)
    {
    }
    template <class VIn, class VOut>
    void operator()(const VIn& vIn, VOut& vOut)
    {
        _gOut[vOut] = _gIn[vIn];
    }

private:
    const GraphIn& _gIn;
    GraphOut& _gOut;
};
}

template <typename VERTEX, typename EDGE, typename OutEdgeList = boost::multisetS, typename VertexList = boost::vecS,
          typename EdgeList = boost::listS>
class InternalGraph
{
public:
    typedef VERTEX Vertex;
    typedef EDGE Edge;
    typedef InternalGraph<Vertex, Edge, OutEdgeList, VertexList, EdgeList> This;

    // Directed acyclic graph
    typedef boost::adjacency_list<OutEdgeList,           // disallow parallel edges (OutEdgeList), use hash_setS ?
                                  VertexList,            // vertex container (VertexList)
                                  boost::bidirectionalS, // directed graph
                                  Vertex, Edge,
                                  boost::no_property, // no GraphProperty
                                  EdgeList            // EdgeList
                                  > GraphContainer;

    // a bunch of graph-specific typedefs
    typedef typename boost::graph_traits<GraphContainer>::vertex_descriptor vertex_descriptor;
    typedef typename boost::graph_traits<GraphContainer>::edge_descriptor edge_descriptor;

    typedef typename boost::graph_traits<GraphContainer>::vertex_iterator vertex_iterator;
    typedef typename boost::graph_traits<GraphContainer>::edge_iterator edge_iterator;
    typedef typename boost::graph_traits<GraphContainer>::adjacency_iterator adjacency_iterator;
    typedef typename boost::graph_traits<GraphContainer>::out_edge_iterator out_edge_iterator;
    typedef typename boost::graph_traits<GraphContainer>::in_edge_iterator in_edge_iterator;

    typedef typename boost::graph_traits<GraphContainer>::degree_size_type degree_t;

    typedef std::pair<adjacency_iterator, adjacency_iterator> adjacency_vertex_range_t;
    typedef std::pair<out_edge_iterator, out_edge_iterator> out_edge_range_t;
    typedef std::pair<in_edge_iterator, in_edge_iterator> in_edge_range_t;
    typedef std::pair<vertex_iterator, vertex_iterator> vertex_range_t;
    typedef std::pair<edge_iterator, edge_iterator> edge_range_t;

    typedef typename Vertex::Key VertexKey;

    // constructors etc.
    InternalGraph() {}

    InternalGraph(const This& g) { *this = g; }

    template <typename V, typename E, typename OEL, typename VL, typename EL>
    InternalGraph(const InternalGraph<V, E, OEL, VL, EL>& g)
    {
        *this = g;
    }

    This& operator=(const This& g)
    {
        if(this == &g)
            return *this;
        clear();
        boost::copy_graph(g.getGraph(), _graph);
        rebuildVertexDescriptorMap();
        return *this;
    }

    template <typename V, typename E, typename OEL, typename VL, typename EL>
    This& operator=(const InternalGraph<V, E, OEL, VL, EL>& g)
    {
        detail::Copier<typename InternalGraph<V, E, OEL, VL, EL>::GraphContainer, GraphContainer> copier(g.getGraph(),
                                                                                                         _graph);
        boost::copy_graph(g.getGraph(), _graph, boost::vertex_copy(copier).edge_copy(copier));
        rebuildVertexDescriptorMap();
        return *this;
    }

    virtual ~InternalGraph() {}

    // structure modification methods
    void clear()
    {
        _graph.clear();
        _vertexDescriptorMap.clear();
    }

    std::size_t getNbEdges() const { return boost::num_edges(_graph); }

    std::size_t getNbVertices() const { return boost::num_vertices(_graph); }

    vertex_descriptor addVertex(const Vertex& prop)
    {
        vertex_descriptor vd = boost::add_vertex(prop, _graph);

        // TUTTLE_LOG_INFO( "addVertex, vd: " << vd << ", prop.getKey(): " << prop.getKey() );
        _vertexDescriptorMap[prop.getKey()] = vd;
        return vd;
    }

    /**
     * @brief Remove in and out edges of a Vertex.
     */
    void clearVertex(const vertex_descriptor& vd)
    {
        boost::clear_vertex(vd, _graph); // remove in and out edges
    }
    void clearVertexInputs(const vertex_descriptor& vd)
    {
        boost::clear_in_edges(vd, _graph); // remove in and out edges
    }
    void clearVertexOutputs(const vertex_descriptor& vd)
    {
        boost::clear_out_edges(vd, _graph); // remove in and out edges
    }

    /**
     * @brief Remove a Vertex.
     */
    void removeVertex(const vertex_descriptor& vd)
    {
        // clear_vertex is not called by boost graph itself.
        // It may result in an undefined behaviour if not called before.
        clearVertex(vd);
        boost::remove_vertex(vd, _graph); // finally remove the vertex from the boost graph
        rebuildVertexDescriptorMap();
    }

    void connect(const VertexKey& out, const VertexKey& in, const std::string& inAttr)
    {
        try
        {
            const vertex_descriptor descOut = getVertexDescriptor(out);
            const vertex_descriptor descIn = getVertexDescriptor(in);

            const Edge e(out, in, inAttr);
            addEdge(descOut, descIn, e);
        }
        catch(boost::exception& e)
        {
            e << exception::user() + "Error while connecting " + out + " --> " + in + "." + inAttr;
            throw;
        }
    }

    void unconnect(const VertexKey& out, const VertexKey& in, const std::string& inAttr)
    {
        try
        {
            const edge_descriptor ed = getEdge(out, in, inAttr);
            removeEdge(ed);
        }
        catch(boost::exception& e)
        {
            e << exception::user() + "No such connection in the graph. Can't unconnect from \"" + getVertex(out) +
                     "\" to \"" + getVertex(in) + "." + inAttr + "\"";
        }
    }

    bool hasEdge(const VertexKey& out, const VertexKey& in, const std::string& inAttr) const
    {
        return hasEdge(getVertexDescriptor(out), getVertexDescriptor(in), inAttr);
    }

    bool hasEdge(const vertex_descriptor& out, const vertex_descriptor& in, const std::string& inAttr) const
    {
        const out_edge_range_t edges = getEdges(out, in);
        BOOST_FOREACH(const edge_descriptor ed, edges)
        {
            if(instance(ed).getInAttrName() == inAttr)
            {
                return true;
            }
        }
        return false;
    }

    edge_descriptor getEdge(const VertexKey& out, const VertexKey& in, const std::string& inAttr) const
    {
        const out_edge_range_t edges = getEdges(getVertexDescriptor(out), getVertexDescriptor(in));
        BOOST_FOREACH(const edge_descriptor ed, edges)
        {
            if(instance(ed).getInAttrName() == inAttr)
            {
                return ed;
            }
        }
        BOOST_THROW_EXCEPTION(exception::Logic() << exception::user() + "No connection in the graph from \"" +
                                                        getVertex(out) + "\" to \"" + getVertex(in) + "/" + inAttr + "\"");
    }

    out_edge_range_t getEdges(const vertex_descriptor& v1, const vertex_descriptor& v2) const
    {
        return boost::edge_range(v1, v2, _graph);
    }

    edge_descriptor addEdge(const vertex_descriptor& v1, const vertex_descriptor& v2, const Edge& prop)
    {
        if(hasEdge(v1, v2, prop.getInAttrName()))
        {
            BOOST_THROW_EXCEPTION(exception::Logic()
                                  << exception::dev() + "Can't add Edge. There is already a connection from \"" +
                                         instance(v1) + "\" to \"" + instance(v2) + "/" + prop.getInAttrName() + "\"");
        }
        // TUTTLE_LOG_VAR2( TUTTLE_TRACE, v1, instance(v1) );
        // TUTTLE_LOG_VAR2( TUTTLE_TRACE, v2, instance(v2) );

        const edge_descriptor addedEdge = boost::add_edge(v1, v2, prop, _graph).first;

        if(hasCycle())
        {
            removeEdge(addedEdge);
            BOOST_THROW_EXCEPTION(exception::Logic()
                                  << exception::user() + "Connection error: the graph becomes cyclic while connecting \"" +
                                         instance(v1) + "\" to \"" + instance(v2) + "\"");
        }
        return addedEdge;
    }

    void removeEdge(const edge_descriptor& e) { boost::remove_edge(e, _graph); }

    vertex_descriptor& getVertexDescriptor(const VertexKey& vertexKey) { return _vertexDescriptorMap[vertexKey]; }

    const vertex_descriptor& getVertexDescriptor(const VertexKey& vertexKey) const
    {
        return _vertexDescriptorMap.find(vertexKey)->second;
    }

    Vertex& getVertex(const VertexKey& vertexKey) { return instance(getVertexDescriptor(vertexKey)); }

    const Vertex& getVertex(const VertexKey& vertexKey) const { return instance(getVertexDescriptor(vertexKey)); }

    const vertex_descriptor source(const edge_descriptor& e) const { return boost::source(e, _graph); }

    Vertex& sourceInstance(const edge_descriptor& e) { return instance(source(e)); }

    const Vertex& sourceInstance(const edge_descriptor& e) const { return instance(source(e)); }

    const vertex_descriptor target(const edge_descriptor& e) const { return boost::target(e, _graph); }

    Vertex& targetInstance(const edge_descriptor& e) { return instance(target(e)); }

    const Vertex& targetInstance(const edge_descriptor& e) const { return instance(target(e)); }

    // property access
    Vertex& instance(const vertex_descriptor& v) { return _graph[v]; }

    const Vertex& instance(const vertex_descriptor& v) const { return _graph[v]; }

    Edge& instance(const edge_descriptor& e) { return _graph[e]; }

    const Edge& instance(const edge_descriptor& e) const { return _graph[e]; }

    GraphContainer& getGraph() { return _graph; }

    const GraphContainer& getGraph() const { return _graph; }

    edge_range_t getEdges() { return edges(_graph); }
    const edge_range_t getEdges() const { return edges(_graph); }

    in_edge_range_t getInEdges(const vertex_descriptor& v) { return in_edges(v, _graph); }
    const in_edge_range_t getInEdges(const vertex_descriptor& v) const { return in_edges(v, _graph); }

    out_edge_range_t getOutEdges(const vertex_descriptor& v) { return out_edges(v, _graph); }
    const out_edge_range_t getOutEdges(const vertex_descriptor& v) const { return out_edges(v, _graph); }

    vertex_range_t getVertices() const { return vertices(_graph); }
    std::vector<vertex_descriptor> getConnectedVertices(const vertex_descriptor& vroot);
    std::vector<vertex_descriptor> getUnconnectedVertices(const vertex_descriptor& vroot);

    adjacency_vertex_range_t getAdjacentVertices(const vertex_descriptor& v) const { return adjacent_vertices(v, _graph); }

    std::size_t getVertexCount() const { return boost::num_vertices(_graph); }

    std::size_t getEdgeCount() const { return boost::num_edges(_graph); }

    degree_t getInDegree(const vertex_descriptor& v) const { return in_degree(v, _graph); }

    degree_t getOutDegree(const vertex_descriptor& v) const { return out_degree(v, _graph); }

    bool hasCycle()
    {
        // we use a depth first search visitor
        bool hasCycle = false;
        visitor::CycleDetector vis(hasCycle);
        this->depthFirstSearch(vis);
        return hasCycle;
    }

    template <class Visitor>
    void depthFirstVisit(Visitor& vis, const vertex_descriptor& vroot)
    {
        std::vector<boost::default_color_type> colormap(boost::num_vertices(_graph), boost::white_color);
        BOOST_FOREACH(const vertex_descriptor& vd, getVertices())
        {
            vis.initialize_vertex(vd, _graph);
        }
        // use depth_first_visit (and not depth_first_search) because
        // we visit vertices from vroot, without visiting nodes not
        // reachable from vroot
        boost::depth_first_visit(_graph, vroot, vis, boost::make_iterator_property_map(
                                                         colormap.begin(), boost::get(boost::vertex_index, _graph)));
    }

    template <class Visitor>
    void depthFirstVisitReverse(Visitor& vis, const vertex_descriptor& vroot)
    {
        boost::reverse_graph<GraphContainer> revGraph(_graph);

        std::vector<boost::default_color_type> colormap(boost::num_vertices(revGraph), boost::white_color);
        BOOST_FOREACH(const vertex_descriptor& vd, vertices(revGraph))
        {
            vis.initialize_vertex(vd, revGraph);
        }

        // use depth_first_visit (and not depth_first_search) because
        // we visit vertices from vroot, without visiting nodes not
        // reachable from vroot
        boost::depth_first_visit(revGraph, vroot, vis, boost::make_iterator_property_map(
                                                           colormap.begin(), boost::get(boost::vertex_index, revGraph)));
    }

    template <class Visitor>
    void depthFirstSearch(Visitor& vis)
    {
        boost::depth_first_search(_graph, boost::visitor(vis));
    }

    template <class Visitor>
    void depthFirstSearchReverse(Visitor& vis)
    {
        boost::reverse_graph<GraphContainer> revGraph(_graph);

        boost::depth_first_search(revGraph, boost::visitor(vis));
    }

    template <class Visitor>
    void depthFirstSearch(Visitor& vis, const vertex_descriptor& vroot)
    {
        boost::depth_first_search(_graph, vroot, boost::visitor(vis));
    }

    template <class Visitor>
    void depthFirstSearchReverse(Visitor& vis, const vertex_descriptor& vroot)
    {
        boost::reverse_graph<GraphContainer> revGraph(_graph);
        boost::depth_first_search(revGraph, vroot, boost::visitor(vis));
    }

    template <class Visitor>
    void breadthFirstSearch(Visitor& vis, const vertex_descriptor& vroot)
    {
        boost::breadth_first_search(_graph, vroot, boost::visitor(vis));
    }

    template <class Visitor>
    void breadthFirstSearch(Visitor& vis)
    {
        boost::breadth_first_search(_graph, boost::visitor(vis));
    }

    void copyTransposed(const This& g)
    {
        // make a transposed copy of g in _graph
        boost::transpose_graph(g._graph, _graph);
        rebuildVertexDescriptorMap();
    }

    template <typename V, typename E, typename OEL, typename VL, typename EL>
    void copyTransposed(const InternalGraph<V, E, OEL, VL, EL>& g)
    {
        detail::Copier<typename InternalGraph<V, E, OEL, VL, EL>::GraphContainer, GraphContainer> copier(g.getGraph(),
                                                                                                         _graph);
        // make a transposed copy of g in _graph
        boost::transpose_graph(g.getGraph(), _graph, boost::vertex_copy(copier).edge_copy(copier));
        rebuildVertexDescriptorMap();
    }

    /**
     * @brief Create a tree from the graph.
     */
    void toDominatorTree();

    /**
     * @brief Create a vector of root vertices, ie. all nodes whithout parents.
     */
    std::vector<vertex_descriptor> rootVertices();

    /**
     * @brief Create a vector of leaf vertices (or external node), ie. all nodes that has zero child node.
     */
    std::vector<vertex_descriptor> leafVertices();

    /**
     * @brief Remove all vertices without connection with vroot.
     */
    std::size_t removeUnconnectedVertices(const vertex_descriptor& vroot);

    template <typename Vertex, typename Edge>
    friend std::ostream& operator<<(std::ostream& os, const This& g);

private:
    void rebuildVertexDescriptorMap();

protected:
    GraphContainer _graph;
    boost::unordered_map<VertexKey, vertex_descriptor> _vertexDescriptorMap;
};
}
}
}

#include "InternalGraph.tcc"

#endif
