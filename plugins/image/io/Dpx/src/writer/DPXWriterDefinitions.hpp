#ifndef _DPXWRITER_DEFINITIONS_HPP_
#define _DPXWRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

enum ETuttlePluginBitDepth
{
	eTuttlePluginBitDepth8 = 0,
	eTuttlePluginBitDepth10,
	eTuttlePluginBitDepth12,
	eTuttlePluginBitDepth16,
	eTuttlePluginBitDepth32,
	eTuttlePluginBitDepth64,
};

static const std::string kParamDescriptor       = "descriptor";
static const std::string kParamDescriptorLabel  = "Descriptor";
static const std::string kParamDescriptorHint   =
	"Descriptor\n"
	"userDefined: User defined descriptor\n"
	"red: Red\n"
	"green: Green\n"
	"blue: Blue\n"
	"alpha: Alpha\n"
	"luma: Luma (Y)\n"
	"colorDifference: Color difference\n"
	"depth: Depth\n"
	"compositeVideo: Composite video\n"
	"rgb: R,G,B\n"
	"rgba; R,G,B,A\n"
	"abgr: A,B,G,R\n"
	"cbycry: Cb,Y,Cr,Y (4:2:2)\n"
	"cbyacrya: Cb,Y,A,Cr,Y,A (4:2:2:4)\n"
	"cbycr: Cb,Y,Cr (4:4:4)\n"
	"cbycra: Cb,Y,Cr,A (4:4:4:4)\n"
	"2componentElement: User defined 2 component element\n"
	"3componentElement: User defined 3 component element\n"
	"4componentElement: User defined 4 component element\n"
	"5componentElement: User defined 5 component element\n"
	"6componentElement: User defined 6 component element\n"
	"7componentElement: User defined 7 component element\n"
	"8componentElement: User defined 8 component element\n"
	"undefined: Undefined descriptor\n";

static const std::string kParamDescriptorUserDefinedDescriptor = "userDefined";
static const std::string kParamDescriptorRed      = "red";
static const std::string kParamDescriptorGreen    = "green";
static const std::string kParamDescriptorBlue     = "blue";
static const std::string kParamDescriptorAlpha    = "alpha";
static const std::string kParamDescriptorLuma     = "luma";
static const std::string kParamDescriptorColorDifference = "colorDifference";
static const std::string kParamDescriptorDepth    = "depth";
static const std::string kParamDescriptorCompositeVideo  = "compositeVideo";
static const std::string kParamDescriptorRGB      = "rgb";
static const std::string kParamDescriptorRGBA     = "rgba";
static const std::string kParamDescriptorABGR     = "abgr";
static const std::string kParamDescriptorCbYCrY   = "cbycry";
static const std::string kParamDescriptorCbYACrYA = "cbyacrya";
static const std::string kParamDescriptorCbYCr    = "cbycr";
static const std::string kParamDescriptorCbYCrA   = "cbycra";
static const std::string kParamDescriptorUserDefined2Comp = "2componentElement";
static const std::string kParamDescriptorUserDefined3Comp = "3componentElement";
static const std::string kParamDescriptorUserDefined4Comp = "4componentElement";
static const std::string kParamDescriptorUserDefined5Comp = "5componentElement";
static const std::string kParamDescriptorUserDefined6Comp = "6componentElement";
static const std::string kParamDescriptorUserDefined7Comp = "7componentElement";
static const std::string kParamDescriptorUserDefined8Comp = "8componentElement";
static const std::string kParamDescriptorUndefinedDescriptor =  "undefined";

static const std::string kParamCharacteristicHint =
	"user defined: User defined\n"
	"density: Printing density\n"
	"linear: Linear, transfer only\n"
	"logarithmic: Logarithmic, transfer only\n"
	"unspecifiedVideo: Unspecified video\n"
	"smpte-274m: SMPTE 274M\n"
	"itu-r709-4: ITU-R 709-4\n"
	"itu-r601-5system-b-or-g: ITU-R 601-5 system B or G\n"
	"itu-r601-5system-m: ITU-R 601-5 system M\n"
	"ntsc: NTSC composite video\n"
	"pal: PAL composite video\n"
	"zdepthlinear: Z depth linear, transfer only\n"
	"zdepthhomogeneous: Z depth homogeneous, transfer only\n"
	"undefined: Undefined\n";

static const std::string kParamTransfer = "transfer";
static const std::string kParamTransferLabel = "Transfer";
static const std::string kParamTransferHint = "Transfer\n" + kParamCharacteristicHint;

static const std::string kParamColorimetric = "colorimetric";
static const std::string kParamColorimetricLabel = "Colorimetric";
static const std::string kParamColorimetricHint = "Colorimetric\n" + kParamCharacteristicHint;

static const std::string kParamCharacteristicUserDefined = "userDefined";
static const std::string kParamCharacteristicPrintingDensity = "density";
static const std::string kParamCharacteristicLinear = "linear";
static const std::string kParamCharacteristicLogarithmic = "logarithmic";
static const std::string kParamCharacteristicUnspecifiedVideo = "unspecifiedVideo";
static const std::string kParamCharacteristicSMPTE274M = "smpte-274m";
static const std::string kParamCharacteristicITUR709 = "itu-r709-4";
static const std::string kParamCharacteristicITUR601 = "itu-r601-5system-b-or-g";
static const std::string kParamCharacteristicITUR602 = "itu-r601-5system-m";
static const std::string kParamCharacteristicNTSCCompositeVideo = "ntsc";
static const std::string kParamCharacteristicPALCompositeVideo = "pal";
static const std::string kParamCharacteristicZLinear = "zdepthlinear";
static const std::string kParamCharacteristicZHomogeneous = "zdepthhomogeneous";
static const std::string kParamCharacteristicUndefinedCharacteristic ="undefined";


static const std::string kParamPacked      = "packed";
static const std::string kParamPackedLabel = "Packed";
static const std::string kParamPackedHint  =
	"Packed\n"
	"packed:  Packed into 32-bit words"
	"a: Filled to 32-bit words, method A"
	"b: Filled to 32-bit words, method B (deprecated)"
;

static const std::string kParamPackedPacked  = "packed";
static const std::string kParamPackedMethodA = "a";
static const std::string kParamPackedMethodB = "b";

static const std::string kParamEncoding  = "encoding";
static const std::string kParamEncodingLabel  = "Encoding";
static const std::string kParamEncodingHint  = "Encoding mode: no encoding (none) or Run Length Encoding (rle)";
static const std::string kParamEncodingNone = "none";
static const std::string kParamEncodingRle = "rle";


}
}
}
}

#endif
