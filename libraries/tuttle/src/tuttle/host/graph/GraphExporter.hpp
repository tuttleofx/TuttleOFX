#ifndef _TUTTLE_GRAPHEXPORTER_HPP_
#define _TUTTLE_GRAPHEXPORTER_HPP_

#include <tuttle/host/graph/InternalGraph.hpp>
#include <tuttle/host/graph/Vertex.hpp>
#include <tuttle/host/graph/Edge.hpp>

#include <boost/graph/graphviz.hpp>
#include <iostream>

namespace tuttle{
	namespace host{
		namespace graph{

template < typename VERTEX, typename EDGE >
struct GraphExporter
{
	static void exportAsDOT(const InternalGraph<VERTEX,EDGE > & g, const char * filename)
	{
		std::ofstream ofs(filename);
		boost::write_graphviz( ofs, g.getGraph() );
	}
};

template <>
struct GraphExporter<Vertex, Edge >
{
	static void exportAsDOT(const InternalGraph<Vertex,Edge > & g, const char * filename)
	{
		std::map<std::string,std::string> graph_attr, vertex_attr, edge_attr;
		graph_attr["size"] 			= "6,6";
		graph_attr["rankdir"] 		= "LR";
		graph_attr["ratio"] 		= "fill";
		graph_attr["label"] 		= "TuttleOFX";
		vertex_attr["shape"] 		= "circle";
		vertex_attr["color"] 		= "dodgerblue4";
		vertex_attr["fontcolor"] 	= "dodgerblue4";
		edge_attr["style"] 			= "dashed";
		edge_attr["minlen"] 		= "1";
		edge_attr["color"] 			= "darkslategray";
		edge_attr["fontcolor"] 		= "darkslategray";

		using namespace boost;
		std::ofstream ofs(filename);
		boost::write_graphviz( ofs
					  , g.getGraph()
					  , boost::make_label_writer(get(vertex_properties, g.getGraph()))
					  , boost::make_label_writer(get(edge_properties, g.getGraph()))
					  , boost::make_graph_attributes_writer(graph_attr, vertex_attr, edge_attr) );
	}
};

} // namespace graph
} // namespace host
} // namespace tuttle

#endif

