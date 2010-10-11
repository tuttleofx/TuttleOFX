#include "Edge.hpp"
#include "GraphExporter.hpp"

namespace tuttle {
namespace host {
namespace graph {

Edge::Edge()
{}

Edge::Edge( const std::string& out, const std::string& in, const std::string& inAttrName )
: IEdge( out, in, inAttrName )
{
}

Edge::Edge( const Edge& e )
: IEdge( e )
{}

Edge::~Edge()
{}


}
}
}
