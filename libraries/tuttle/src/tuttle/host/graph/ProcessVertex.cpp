#include "ProcessVertex.hpp"
#include "GraphExporter.hpp"

#include <tuttle/host/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace graph {


ProcessVertex::ProcessVertex( const std::string& name )
: IVertex( name )
{
}

ProcessVertex::ProcessVertex( const UVertex& v )
: IVertex( v )
, _data( v.getProcessNode().getNodeType() )
{
}

ProcessVertex::ProcessVertex( const ProcessVertex& v )
: IVertex( v )
, _data( v._data )
{
}



std::ostream& ProcessVertex::exportDotDebug( std::ostream& os ) const
{
	std::ostringstream s;
	s << subDotEntry( "label", getName() );
	if( ! isFake() )
	{
		/// @todo remove this. Temporary solution
		s << subDotEntry( "bitdepth", static_cast<const ImageEffectNode&>( getProcessNode() ).getOutputClip().getBitDepthString() );
	}
	s << subDotEntry( "allTimes", _data._times.size() );
	std::ostringstream times;
	std::copy(
        _data._times.begin(),
        _data._times.end(),
        std::ostream_iterator<OfxTime>( times, "," ) );
	s << subDotEntry( "times", times.str() );

	os << "[" << std::endl;
	os << dotEntry( "type", "Node" ) << ", " << std::endl;
	os << dotEntry( "label", s.str() ) << ", " << std::endl;
	os << "]" << std::endl;
	return os;
	return os;
}

std::ostream& operator<<( std::ostream& os, const ProcessVertex& v )
{
	return operator<<( os, static_cast<const IVertex&>(v) );
}

}
}
}
