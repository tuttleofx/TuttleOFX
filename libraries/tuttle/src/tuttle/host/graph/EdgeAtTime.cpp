#include "EdgeAtTime.hpp"
#include "GraphExporter.hpp"

namespace tuttle {
namespace host {
namespace graph {

EdgeAtTime::EdgeAtTime()
{
}

EdgeAtTime::EdgeAtTime( const VertexAtTime::Key& out, const VertexAtTime::Key& in, const std::string& inAttrName )
: IEdge( out.first, in.first, inAttrName )
, _inTime( in.second )
, _outTime( out.second )
{
}

EdgeAtTime::EdgeAtTime( const Edge& e )
: IEdge( e )
{
}

EdgeAtTime::EdgeAtTime( const EdgeAtTime& other )
: IEdge( other )
, _inTime( other._inTime )
, _outTime( other._outTime )
{
}

EdgeAtTime::~EdgeAtTime()
{
}

std::ostream& EdgeAtTime::exportDotDebug( std::ostream& os ) const
{
	std::ostringstream s;
	s << subDotEntry( "label", getName() );
	s << subDotEntry( "id", _localId );


	std::ostringstream times;
	times << "(in:" << _inTime << ", out:" << _outTime << ")";
	s << subDotEntry( "times", times.str() );

	os << "[" << std::endl;
	os << dotEntry( "type", "Edge" ) << ", " << std::endl;
	os << dotEntry( "label", s.str() ) << std::endl;
	os << "]" << std::endl;
	return os;
}


}
}
}
