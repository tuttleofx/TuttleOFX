#include "Vertex.hpp"
#include "GraphExporter.hpp"


namespace tuttle {
namespace host {
namespace graph {


Vertex::Vertex( const std::string& name )
: IVertex( name )
{
}

Vertex::Vertex( const std::string& name, INode& processNode )
: IVertex( name, processNode )
{
}

Vertex::Vertex( const Vertex& v )
: IVertex( v )
, _times( v._times )
{
}



std::ostream& Vertex::exportDotDebug( std::ostream& os ) const
{
	IVertex::exportDotDebug( os );
//	os << "(allTimes=" << _times.size() << "\"";
//	std::copy(
//        _times.begin(),
//        _times.end(),
//        std::ostream_iterator<OfxTime>( os, "," ) );
//	os << "\")\n";
	return os;
}

std::ostream& operator<<( std::ostream& os, const Vertex& v )
{
	return operator<<( os, static_cast<const IVertex&>(v) );
}

}
}
}
