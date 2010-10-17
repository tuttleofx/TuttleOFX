#include "EdgeAtTime.hpp"

namespace tuttle {
namespace host {
namespace graph {

EdgeAtTime::EdgeAtTime()
{
}

EdgeAtTime::EdgeAtTime( const VertexAtTime::Key& out, const VertexAtTime::Key& in, const std::string& inAttrName )
: IEdge( out.first, in.first, inAttrName )
{
}

EdgeAtTime::EdgeAtTime( const Edge& e )
: IEdge( e )
{
}

EdgeAtTime::EdgeAtTime( const EdgeAtTime& other )
: IEdge( other )
{
}

EdgeAtTime::~EdgeAtTime()
{
}


}
}
}
