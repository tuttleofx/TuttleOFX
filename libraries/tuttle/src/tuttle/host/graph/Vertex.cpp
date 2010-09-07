#include "Vertex.hpp"
#include "GraphExporter.hpp"

#include <tuttle/host/ImageEffectNode.hpp>

namespace tuttle {
namespace host {
namespace graph {

int Vertex::_count = 0;

std::ostream& Vertex::exportDotDebug( std::ostream& os ) const
{
	os << "[" << std::endl;
	os << dotEntry( "type", "Node" ) << ", " << std::endl;
	os << dotEntry( "label", getName() ) << ", " << std::endl;
	if( ! isFake() )
	{
		os << dotEntry( "bitdepth", static_cast<const ImageEffectNode*>( getProcessNode() )->getOutputClip().getBitDepthString() ) << ", " << std::endl;
	}
	os << dotEntry( "localMemory", getProcessOptions()._localInfos._memory ) << ", " << std::endl;
	os << dotEntry( "globalMemory", getProcessOptions()._globalInfos._memory ) << std::endl;
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
