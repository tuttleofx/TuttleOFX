#include "VertexAtTime.hpp"
#include "GraphExporter.hpp"

#include <tuttle/host/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace graph {

VertexAtTime::VertexAtTime( )
{ }

VertexAtTime::VertexAtTime( const Vertex& v, const OfxTime t )
: IVertex( v )
, _data( v._data, t )
{ }

VertexAtTime::VertexAtTime( const VertexAtTime& other )
: IVertex( other )
, _data( other._data )
{ }

VertexAtTime::~VertexAtTime( )
{ }



std::ostream& VertexAtTime::exportDotDebug( std::ostream& os ) const
{
	std::ostringstream s;
	s << subDotEntry( "label", getName() );
	if( ! isFake() )
	{
		/// @todo remove this. Temporary solution
		s << subDotEntry( "bitdepth", static_cast<const ImageEffectNode*>( getProcessNode() )->getOutputClip().getBitDepthString() );
	}
	s << subDotEntry( "localMemory", _data._localInfos._memory );
	s << subDotEntry( "globalMemory", _data._globalInfos._memory );
	s << subDotEntry( "time", _data._time );

	os << "[" << std::endl;
	os << dotEntry( "type", "Node" ) << ", " << std::endl;
	os << dotEntry( "label", s.str() ) << ", " << std::endl;
	os << "]" << std::endl;
	return os;
}


std::ostream& operator<<( std::ostream& os, const VertexAtTime& v )
{
	os << v.getName() << "_" << v._data._time;
	return os;
}

std::ostream& operator<<( std::ostream& os, const VertexAtTime::Key& v )
{
	os << v.first << "_" << v.second;
	return os;
}

}
}
}
