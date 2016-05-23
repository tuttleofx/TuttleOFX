#ifndef _DPXWRITER_DEFINITIONS_HPP_
#define _DPXWRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace dpx
{
namespace writer
{

enum ETuttlePluginBitDepth
{
    eTuttlePluginBitDepth8 = 0,
    eTuttlePluginBitDepth10,
    eTuttlePluginBitDepth12,
    eTuttlePluginBitDepth16,
    eTuttlePluginBitDepth32,
    eTuttlePluginBitDepth64,
};

static const std::string kParamDescriptorHint = "Select Components\n"
                                                "auto: Luma for 1 channel, RGB for 3 channels and RGBA for 4 channels\n";

static const std::string kParamDescriptorUserDefinedDescriptor = "userDefined";
static const std::string kParamDescriptorRed = "red";
static const std::string kParamDescriptorGreen = "green";
static const std::string kParamDescriptorBlue = "blue";
static const std::string kParamDescriptorAlpha = "alpha";
static const std::string kParamDescriptorLuma = "luma / Y";
static const std::string kParamDescriptorColorDifference = "colorDifference";
static const std::string kParamDescriptorDepth = "depth";
static const std::string kParamDescriptorCompositeVideo = "compositeVideo";
static const std::string kParamDescriptorRGB = "rgb";
static const std::string kParamDescriptorRGBA = "rgba";
static const std::string kParamDescriptorABGR = "abgr";
static const std::string kParamDescriptorCbYCrY = "cbycry   (4:2:2)";
static const std::string kParamDescriptorCbYACrYA = "cbyacrya (4:2:2:4)";
static const std::string kParamDescriptorCbYCr = "cbycr    (4:4:4)";
static const std::string kParamDescriptorCbYCrA = "cbycra   (4:4:4:4)";
static const std::string kParamDescriptorUserDefined2Comp = "2componentElement";
static const std::string kParamDescriptorUserDefined3Comp = "3componentElement";
static const std::string kParamDescriptorUserDefined4Comp = "4componentElement";
static const std::string kParamDescriptorUserDefined5Comp = "5componentElement";
static const std::string kParamDescriptorUserDefined6Comp = "6componentElement";
static const std::string kParamDescriptorUserDefined7Comp = "7componentElement";
static const std::string kParamDescriptorUserDefined8Comp = "8componentElement";
static const std::string kParamDescriptorUndefinedDescriptor = "undefined";
static const std::string kParamDescriptorAuto = "auto";

static const std::string kParamCharacteristicHint = "* transfer only\n";

static const std::string kParamTransfer = "transfer";
static const std::string kParamTransferLabel = "Transfer";
static const std::string kParamTransferHint = "Transfer\n" + kParamCharacteristicHint;

static const std::string kParamColorimetric = "colorimetric";
static const std::string kParamColorimetricLabel = "Colorimetric";
static const std::string kParamColorimetricHint = "Colorimetric\n" + kParamCharacteristicHint;

static const std::string kParamCharacteristicUserDefined = "userDefined";
static const std::string kParamCharacteristicPrintingDensity = "printingDensity";
static const std::string kParamCharacteristicLinear = "linear *";
static const std::string kParamCharacteristicLogarithmic = "logarithmic *";
static const std::string kParamCharacteristicUnspecifiedVideo = "unspecifiedVideo";
static const std::string kParamCharacteristicSMPTE274M = "smpte-274m";
static const std::string kParamCharacteristicITUR709 = "itu-r709-4";
static const std::string kParamCharacteristicITUR601 = "itu-r601-5system-b-or-g";
static const std::string kParamCharacteristicITUR602 = "itu-r601-5system-m";
static const std::string kParamCharacteristicNTSCCompositeVideo = "ntsc";
static const std::string kParamCharacteristicPALCompositeVideo = "pal";
static const std::string kParamCharacteristicZLinear = "zdepthlinear *";
static const std::string kParamCharacteristicZHomogeneous = "zdepthhomogeneous *";
static const std::string kParamCharacteristicUndefinedCharacteristic = "undefined";

static const std::string kParamPacked = "packed";
static const std::string kParamPackedLabel = "Packed";
static const std::string kParamPackedHint = "Packed or filled mode into 32-bit words";

static const std::string kParamPackedPacked = "packed";
static const std::string kParamPackedMethodA = "a";
static const std::string kParamPackedMethodB = "b (deprecated)";

static const std::string kParamSwapEndian = "swapendian";
static const std::string kParamSwapEndianLabel = "Swap Endian";
static const std::string kParamSwapEndianHint = "Swap endian";

static const std::string kParamEncoding = "encoding";
static const std::string kParamEncodingLabel = "Encoding";
static const std::string kParamEncodingHint = "Encoding mode: no encoding (none) or Run Length Encoding (rle)";
static const std::string kParamEncodingNone = "none";
static const std::string kParamEncodingRle = "rle";

static const std::string kParamOrientation = "orientation";
static const std::string kParamOrientationLabel = "Orientation";
static const std::string kParamOrientationHint = "Orientation mode: select image orientation";
static const std::string kParamOrientationLeftToRightTopToBottom = "lefttorighttoptobottom";
static const std::string kParamOrientationRightToLeftTopToBottom = "righttolefttoptobottom";
static const std::string kParamOrientationLeftToRightBottomToTop = "bottomtotoplefttorightbtt";
static const std::string kParamOrientationRightToLeftBottomToTop = "bottomtotoprighttoleft";
static const std::string kParamOrientationTopToBottomLeftToRight = "toptobottomlefttoright";
static const std::string kParamOrientationTopToBottomRightToLeft = "toptobottomrighttoleft";
static const std::string kParamOrientationBottomToTopLeftToRight = "bottomtotoplefttoright";
static const std::string kParamOrientationBottomToTopRightToLeft = "bottomtotoprighttoleft";
static const std::string kParamOrientationUndefinedOrientation = "undefined";

static const std::string kParamProject = "project";
static const std::string kParamCopyright = "copyright";
}
}
}
}

#endif
