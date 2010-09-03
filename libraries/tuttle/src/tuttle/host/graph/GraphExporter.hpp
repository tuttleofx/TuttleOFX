#ifndef _TUTTLE_GRAPHEXPORTER_HPP_
#define _TUTTLE_GRAPHEXPORTER_HPP_

#include <tuttle/host/graph/InternalGraph.hpp>
#include <tuttle/host/graph/Vertex.hpp>
#include <tuttle/host/graph/Edge.hpp>

#include <boost/graph/graphviz.hpp>
#include <iostream>

namespace tuttle {
namespace host {
namespace graph {

template <class Name>
class node_writer {
public:
	node_writer(Name _name) : name(_name) {}
	template <class VertexOrEdge>
	void operator()(std::ostream& out, const VertexOrEdge& v) const
	{
		out << " [ " << get(name, v) << " ]";
	}
private:
	Name name;
};

template <class Name>
inline node_writer<Name>
make_node_writer(Name n)
{
	return node_writer<Name>(n);
}

template<typename Vertex, typename Edge>
inline void exportSimple( std::ostream& os, const InternalGraph<Vertex, Edge>& g )
{
	using namespace boost;
	boost::write_graphviz( os, g.getGraph(),
	                           make_node_writer( get( &Vertex::_name, g.getGraph() ) ),
	                           make_node_writer( get( &Edge::_name,   g.getGraph() ) ) );
}

template<typename Vertex, typename Edge>
inline void exportAsDOT( std::ostream& os, const InternalGraph<Vertex, Edge >& g )
{
	boost::write_graphviz( os, g.getGraph() );
}

template<>
inline void exportAsDOT<Vertex, Edge >( std::ostream& os, const InternalGraph<Vertex, Edge>& g )
{
	std::map<std::string, std::string> graph_attr, vertex_attr, edge_attr;
	graph_attr["size"]       = "6,6";
	graph_attr["rankdir"]    = "LR";
	graph_attr["ratio"]      = "fill";
	graph_attr["label"]      = "TuttleOFX";
	vertex_attr["shape"]     = "circle";
	vertex_attr["color"]     = "dodgerblue4";
	vertex_attr["fontcolor"] = "dodgerblue4";
	edge_attr["style"]       = "dashed";
	edge_attr["minlen"]      = "1";
	edge_attr["color"]       = "darkslategray";
	edge_attr["fontcolor"]   = "darkslategray";

	using namespace boost;
	boost::write_graphviz( os,
						   g.getGraph(),
						   boost::make_label_writer( get( &Vertex::_name, g.getGraph() ) ),
						   boost::make_label_writer( get( &Edge::_name, g.getGraph() ) ),
						   boost::make_graph_attributes_writer( graph_attr, vertex_attr, edge_attr ) );
}

template<typename Vertex, typename Edge>
inline void exportAsDOT( const std::string& filename, const InternalGraph<Vertex, Edge >& g )
{
	std::ofstream ofs( filename.c_str() );
	exportAsDOT( ofs, g );
}


}
}
}

#endif

