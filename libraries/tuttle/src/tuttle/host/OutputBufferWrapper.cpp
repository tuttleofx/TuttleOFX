#include "OutputBufferWrapper.hpp"
#include "Core.hpp"
#include <tuttle/host/ofx/attribute/OfxhClipImageDescriptor.hpp>

namespace tuttle {
namespace host {

void OutputBufferWrapper::setCallback( CallbackOutputImagePtr callback, CustomDataPtr customData, CallbackDestroyCustomDataPtr destroyCustomData )
{
	_node.getParam( "callbackPointer" ).setValue(
			boost::lexical_cast<std::string>( reinterpret_cast<std::ptrdiff_t>( callback ) )
		);
	_node.getParam( "customData" ).setValue(
			boost::lexical_cast<std::string>( reinterpret_cast<std::ptrdiff_t>( customData ) )
		);
	_node.getParam( "callbackDestroyCustomData" ).setValue(
			boost::lexical_cast<std::string>( reinterpret_cast<std::ptrdiff_t>( destroyCustomData ) )
		);
}

}
}
