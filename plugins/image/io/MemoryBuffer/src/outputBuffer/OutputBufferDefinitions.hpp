#ifndef _TUTTLE_PLUGIN_OUTPUTBUFFER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_OUTPUTBUFFER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace outputBuffer
{

static const std::string kParamOutputCallbackPointer = "callbackPointer";
static const std::string kParamOutputCustomData = "customData";
static const std::string kParamOutputCallbackDestroyCustomData = "callbackDestroyCustomData";

extern "C" {
typedef void* CustomDataPtr;
typedef void (*CallbackOutputImagePtr)(OfxTime time, CustomDataPtr outputCustomData, void* rawdata, int width, int height,
                                       int rowSizeBytes, OFX::EBitDepth bitDepth, OFX::EPixelComponent components,
                                       OFX::EField field);
typedef void (*CallbackDestroyCustomDataPtr)(CustomDataPtr outputCustomData);
}
}
}
}

#endif
