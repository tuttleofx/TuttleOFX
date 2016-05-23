#ifndef _TUTTLE_HOST_GRAPHEXPORTER_HPP_
#define _TUTTLE_HOST_GRAPHEXPORTER_HPP_

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
struct DotEntry
{
    DotEntry(const std::string& key, const T& value)
        : _key(key)
        , _value(value)
    {
    }
    const std::string& _key;
    const T& _value;
    template <class TT>
    friend std::ostream& operator<<(std::ostream& os, const DotEntry<TT>& d);
};
template <class T>
struct SubDotEntry : public DotEntry<T>
{
    SubDotEntry(const std::string& key, const T& value)
        : DotEntry<T>(key, value)
    {
    }
    template <class TT>
    friend std::ostream& operator<<(std::ostream& os, const SubDotEntry<TT>& d);
};
}
template <typename Vertex, typename ProcessEdge, typename OutEdgeList, typename VertexList, typename EdgeList>
class InternalGraph;

/**
 * @brief Use this function to force the correct syntax.
 *        os << dotEntry( "label", "fooNode");
 *        output: [label="fooNode"]
 */
template <class T>
detail::DotEntry<T> dotEntry(const std::string& key, const T& value);
template <class T>
detail::SubDotEntry<T> subDotEntry(const std::string& key, const T& value);

template <typename Vertex, typename ProcessEdge, typename OutEdgeList, typename VertexList, typename EdgeList>
inline void exportSimple(std::ostream& os, const InternalGraph<Vertex, ProcessEdge, OutEdgeList, VertexList, EdgeList>& g);

template <typename Vertex, typename ProcessEdge, typename OutEdgeList, typename VertexList, typename EdgeList>
inline void exportAsDOT(std::ostream& os, const InternalGraph<Vertex, ProcessEdge, OutEdgeList, VertexList, EdgeList>& g);

template <typename Vertex, typename ProcessEdge, typename OutEdgeList, typename VertexList, typename EdgeList>
inline void exportAsDOT(const std::string& filename,
                        const InternalGraph<Vertex, ProcessEdge, OutEdgeList, VertexList, EdgeList>& g);

template <typename Vertex, typename ProcessEdge, typename OutEdgeList, typename VertexList, typename EdgeList>
inline void exportDebugAsDOT(std::ostream& os,
                             const InternalGraph<Vertex, ProcessEdge, OutEdgeList, VertexList, EdgeList>& g);

template <typename Vertex, typename ProcessEdge, typename OutEdgeList, typename VertexList, typename EdgeList>
inline void exportDebugAsDOT(const std::string& filename,
                             const InternalGraph<Vertex, ProcessEdge, OutEdgeList, VertexList, EdgeList>& g);
}
}
}

#include "GraphExporter.tcc"

#endif
