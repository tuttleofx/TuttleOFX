#include "ProcessVertexAtTime.hpp"
#include "GraphExporter.hpp"

#include <tuttle/host/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace graph {

ProcessVertexAtTime::ProcessVertexAtTime( )
{ }

ProcessVertexAtTime::ProcessVertexAtTime( const ProcessVertex& v, const OfxTime t )
: IVertex( v )
, _data( v._data, t )
{ }

ProcessVertexAtTime::ProcessVertexAtTime( const ProcessVertexAtTime& other )
: IVertex( other )
, _data( other._data )
{ }

ProcessVertexAtTime::~ProcessVertexAtTime( )
{ }



std::ostream& ProcessVertexAtTime::exportDotDebug( std::ostream& os ) const
{
	std::ostringstream s;
	s << subDotEntry( "label", getName() );
	if( ! isFake() )
	{
		/// @todo remove this. Temporary solution
		const ImageEffectNode& ieNode = dynamic_cast<const ImageEffectNode&>( getProcessNode() );
		s << subDotEntry( "bitdepth",  ieNode.getOutputClip().getBitDepthString()   );
		s << subDotEntry( "component", ieNode.getOutputClip().getComponentsString() );
		{
			double startFrame, endFrame;
			ieNode.getOutputClip().getFrameRange( startFrame, endFrame );
			s << subDotEntry( "startFrame", startFrame );
			s << subDotEntry( "endFrame", endFrame );
		}
		s << subDotEntry( "output RoD", ieNode.getOutputClip().fetchRegionOfDefinition(_data._time) );
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


std::ostream& operator<<( std::ostream& os, const ProcessVertexAtTime& v )
{
	os << v.getName() << "_" << v._data._time;
	return os;
}

std::ostream& operator<<( std::ostream& os, const ProcessVertexAtTime::Key& v )
{
	os << v.first << "_" << v.second;
	return os;
}

}
}
}
