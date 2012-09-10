#include "OutputBufferWrapper.hpp"
#include "Core.hpp"
#include <tuttle/host/ofx/attribute/OfxhClipImageDescriptor.hpp>

namespace tuttle {
namespace host {

void OutputBufferWrapper::setCallback( OutCallbackPtr callback, CallbackCustomDataPtr customData )
{
	_node.getParam( "callbackPointer" ).setValue(
			boost::lexical_cast<std::string>( reinterpret_cast<std::ptrdiff_t>( callback ) )
		);
	_node.getParam( "callbackCustomData" ).setValue(
			boost::lexical_cast<std::string>( reinterpret_cast<std::ptrdiff_t>( customData ) )
		);
}

}
}
