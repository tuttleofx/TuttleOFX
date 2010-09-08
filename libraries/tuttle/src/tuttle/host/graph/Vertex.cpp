#include "Vertex.hpp"
#include "GraphExporter.hpp"

#include <tuttle/host/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace graph {

int Vertex::_count = 0;

std::ostream& Vertex::exportDotDebug( std::ostream& os ) const
{
	std::ostringstream s;
	s << subDotEntry( "label", getName() ) << ", ";
	if( ! isFake() )
	{
		s << subDotEntry( "bitdepth", static_cast<const ImageEffectNode*>( getProcessNode() )->getOutputClip().getBitDepthString() ) << ", ";
	}
	s << subDotEntry( "localMemory", getProcessOptions()._localInfos._memory ) << ", ";
	s << subDotEntry( "globalMemory", getProcessOptions()._globalInfos._memory );

	os << "[" << std::endl;
	os << dotEntry( "type", "Node" ) << ", " << std::endl;
	os << dotEntry( "label", s.str() ) << ", " << std::endl;
	os << "]" << std::endl;
	return os;
}

std::ostream& operator<<( std::ostream& os, const Vertex& v )
{
	os << v.getName() ;
	return os;
}

}
}
}
