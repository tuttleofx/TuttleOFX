#include <tuttle/host/graph/InternalGraph.hpp>
#include <tuttle/host/graph/IVertex.hpp>
#include <tuttle/host/graph/IEdge.hpp>

#include <boost/graph/graphviz.hpp>
#include <iostream>

namespace tuttle
{
namespace host
{
namespace graph
{

namespace detail
{
template <class T>
std::ostream& operator<<(std::ostream& os, const DotEntry<T>& d)
{
    //	os << "[" << d._key << "=\"" << d._value << "\"]";
    os << d._key << "=\"" << d._value << "\"";
    return os;
}
template <class T>
std::ostream& operator<<(std::ostream& os, const SubDotEntry<T>& d)
{
    os << d._key << ":\'" << d._value << "\'\\n";
    return os;
}
}

/**
 * @brief Use this function to force the correct syntax.
 *        os << dotEntry( "label", "fooNode");
 *        output: [label="fooNode"]
 */
template <class T>
detail::DotEntry<T> dotEntry(const std::string& key, const T& value)
{
    return detail::DotEntry<T>(key, value);
}
template <class T>
detail::SubDotEntry<T> subDotEntry(const std::string& key, const T& value)
{
    return detail::SubDotEntry<T>(key, value);
}

namespace detail
{
template <class Name>
class simple_node_writer
{
public:
    simple_node_writer(Name _name)
        : name(_name)
    {
    }
    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& v) const
    {
        out << " [ " << get(name, v) << " ]";
    }

private:
    Name name;
};
}

/**
 * @brief For simple export, don't use the real .dot syntax, just put the name.
 * like: [ fooNode ]
 * instead of: [label="fooNode"]
 */
template <class Name>
inline detail::simple_node_writer<Name> make_simple_node_writer(Name n)
{
    return detail::simple_node_writer<Name>(n);
}

template <typename Vertex, typename ProcessEdge, typename OutEdgeList, typename VertexList, typename EdgeList>
inline void exportSimple(std::ostream& os, const InternalGraph<Vertex, ProcessEdge, OutEdgeList, VertexList, EdgeList>& g)
{
    using namespace boost;
    boost::write_graphviz(os, g.getGraph(), make_simple_node_writer(get(&Vertex::_name, g.getGraph())),
                          make_simple_node_writer(get(&ProcessEdge::_name, g.getGraph())));
}

template <typename Vertex, typename ProcessEdge, typename OutEdgeList, typename VertexList, typename EdgeList>
inline void exportAsDOT(std::ostream& os, const InternalGraph<Vertex, ProcessEdge, OutEdgeList, VertexList, EdgeList>& g)
{
    std::map<std::string, std::string> graph_attr, vertex_attr, edge_attr;
    graph_attr["size"] = "6,6";
    graph_attr["rankdir"] = "LR";
    graph_attr["ratio"] = "fill";
    graph_attr["label"] = "TuttleOFX";
    vertex_attr["shape"] = "circle";
    vertex_attr["color"] = "dodgerblue4";
    vertex_attr["fontcolor"] = "dodgerblue4";
    edge_attr["style"] = "dashed";
    edge_attr["minlen"] = "1";
    edge_attr["color"] = "darkslategray";
    edge_attr["fontcolor"] = "darkslategray";

    using namespace boost;
    boost::write_graphviz(os, g.getGraph(), boost::make_label_writer(get(&IVertex::_name, g.getGraph())),
                          boost::make_label_writer(get(&IEdge::_name, g.getGraph())),
                          boost::make_graph_attributes_writer(graph_attr, vertex_attr, edge_attr));
}

template <typename Vertex, typename ProcessEdge, typename OutEdgeList, typename VertexList, typename EdgeList>
inline void exportAsDOT(const std::string& filename,
                        const InternalGraph<Vertex, ProcessEdge, OutEdgeList, VertexList, EdgeList>& g)
{
    std::ofstream ofs(filename.c_str());

    exportAsDOT(ofs, g);
}

namespace detail
{
template <class Graph>
struct debug_node_writer
{
    debug_node_writer(const Graph& graph)
        : _graph(graph)
    {
    }
    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& vd) const
    {
        _graph.instance(vd).exportDotDebug(out);
    }
    const Graph& _graph;
};
}

template <class Graph>
inline detail::debug_node_writer<Graph> make_debug_node_writer(const Graph g)
{
    return detail::debug_node_writer<Graph>(g);
}

template <typename Vertex, typename ProcessEdge, typename OutEdgeList, typename VertexList, typename EdgeList>
inline void exportDebugAsDOT(std::ostream& os,
                             const InternalGraph<Vertex, ProcessEdge, OutEdgeList, VertexList, EdgeList>& g)
{
    std::map<std::string, std::string> graph_attr, vertex_attr, edge_attr;
    graph_attr["size"] = "6,6";
    graph_attr["rankdir"] = "LR";
    graph_attr["ratio"] = "fill";
    graph_attr["label"] = "TuttleOFX";
    vertex_attr["shape"] = "circle";
    vertex_attr["color"] = "dodgerblue4";
    vertex_attr["fontcolor"] = "dodgerblue4";
    edge_attr["style"] = "dashed";
    edge_attr["minlen"] = "1";
    edge_attr["color"] = "darkslategray";
    edge_attr["fontcolor"] = "darkslategray";

    using namespace boost;
    boost::write_graphviz(os, g.getGraph(), make_debug_node_writer(g), make_debug_node_writer(g),
                          boost::make_graph_attributes_writer(graph_attr, vertex_attr, edge_attr));
}

template <typename Vertex, typename ProcessEdge, typename OutEdgeList, typename VertexList, typename EdgeList>
inline void exportDebugAsDOT(const std::string& filename,
                             const InternalGraph<Vertex, ProcessEdge, OutEdgeList, VertexList, EdgeList>& g)
{
    std::ofstream ofs(filename.c_str());

    exportDebugAsDOT(ofs, g);
}
}
}
}
