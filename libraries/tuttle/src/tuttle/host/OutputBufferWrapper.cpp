#include "OutputBufferWrapper.hpp"
#include "Core.hpp"
#include <tuttle/host/ofx/attribute/OfxhClipImageDescriptor.hpp>

namespace tuttle
{
namespace host
{

void OutputBufferWrapper::setCallback(CallbackOutputImagePtr callback, CustomDataPtr customData,
                                      CallbackDestroyCustomDataPtr destroyCustomData)
{
    getNode()
        .getParam("callbackPointer")
        .setValue(boost::lexical_cast<std::string>(reinterpret_cast<std::ptrdiff_t>(callback)));
    getNode()
        .getParam("customData")
        .setValue(boost::lexical_cast<std::string>(reinterpret_cast<std::ptrdiff_t>(customData)));
    getNode()
        .getParam("callbackDestroyCustomData")
        .setValue(boost::lexical_cast<std::string>(reinterpret_cast<std::ptrdiff_t>(destroyCustomData)));
}
}
}
