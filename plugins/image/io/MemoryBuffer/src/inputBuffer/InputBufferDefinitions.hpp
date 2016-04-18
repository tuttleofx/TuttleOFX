#ifndef _TUTTLE_PLUGIN_INPUTBUFFER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_INPUTBUFFER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace inputBuffer
{

static const std::string kParamInputMode = "mode";
static const std::string kParamInputModeBufferPointer = "bufferPointer";
static const std::string kParamInputModeCallbackPointer = "callbackPointer";

enum EParamInputMode
{
    eParamInputModeBufferPointer = 0,
    eParamInputModeCallbackPointer = 1
};

static const std::string kParamInputBufferPointer = "bufferPointer";
static const std::string kParamInputCallbackPointer = "callbackPointer";
static const std::string kParamInputCustomData = "customData";
static const std::string kParamInputCallbackDestroyCustomData = "callbackDestroyCustomData";

extern "C" {
typedef void* CustomDataPtr;
typedef void (*CallbackInputImagePtr)(OfxTime time, CustomDataPtr customData, void** outRawdata, int* outWidth,
                                      int* outHeight, int* outRowSizeBytes);
typedef void (*CallbackDestroyCustomDataPtr)(CustomDataPtr customData);
}

static const std::string kParamSize = "size";
static const std::string kParamRowBytesSize = "rowBytesSize";
static const std::string kParamPixelAspectRatio = "par";
static const std::string kParamFramerate = "framerate";

static const std::string kParamPixelComponents = "components";
static const std::string kParamPixelComponentsAlpha = "Gray";
static const std::string kParamPixelComponentsRGB = "RGB";
static const std::string kParamPixelComponentsRGBA = "RGBA";

/** @brief Enumerates the component types supported */
enum EParamPixelComponent
{
    eParamPixelComponentAlpha = 0,
    eParamPixelComponentRGB = 1,
    eParamPixelComponentRGBA = 2
};

static const std::string kParamBitDepth = "bitDepth";
static const std::string kParamBitDepthUByte = "UByte";
static const std::string kParamBitDepthUShort = "UShort";
static const std::string kParamBitDepthFloat = "Float";

/** @brief Enumerates the pixel depths supported */
enum EParamBitDepth
{
    eParamBitDepthUByte = 0,
    eParamBitDepthUShort = 1,
    eParamBitDepthFloat = 2
};

static const std::string kParamField = "field";
static const std::string kParamFieldNone = "None";
static const std::string kParamFieldBoth = "Both";
static const std::string kParamFieldLower = "Lower";
static const std::string kParamFieldUpper = "Upper";

/** @brief Enumerates the fields present in an image */
enum EParamField
{
    eParamFieldNone = 0,  /**< @brief unfielded image */
    eParamFieldBoth = 1,  /**< @brief fielded image with both fields present */
    eParamFieldLower = 2, /**< @brief only the spatially lower field is present */
    eParamFieldUpper = 3  /**< @brief only the spatially upper field is present  */
};

static const std::string kParamOrientation = "orientation";
static const std::string kParamOrientationFromBottomToTop = "bottomToTop";
static const std::string kParamOrientationFromTopToBottom = "topToBottom";

/** @brief Enumerates the image memory orientations */
enum EParamOrientation
{
    eParamOrientationFromBottomToTop = 0,
    eParamOrientationFromTopToBottom = 1
};

static const std::string kParamTimeDomain = "timeDomain";
}
}
}

#endif
