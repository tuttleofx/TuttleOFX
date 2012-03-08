#ifndef _OPENIMAGEIOWRITER_DEFINITIONS_HPP_
#define _OPENIMAGEIOWRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace writer {

enum ETuttlePluginBitDepth
{
	eTuttlePluginBitDepth8 = 0,
	eTuttlePluginBitDepth16,
	eTuttlePluginBitDepth16f,
	eTuttlePluginBitDepth32,
	eTuttlePluginBitDepth32f
};

enum ETuttlePluginComponents
{
	eTuttlePluginComponentsGray = 0,
	eTuttlePluginComponentsRGB,
	eTuttlePluginComponentsRGBA
};

static const std::string kParamOutputCompression  = "compression";

static const std::string kParamOutputCompressionNone  = "none   No compression";
static const std::string kParamOutputCompressionZip   = "zip    Compression zlib, with blocs of 16 scanlines.";
static const std::string kParamOutputCompressionZips  = "zips   Compression zlib, with blocs with only one scanline.";
static const std::string kParamOutputCompressionRle   = "rle    Compression Run Length Encoding.";
static const std::string kParamOutputCompressionPiz   = "piz    Piz-based wavelet compression.";
static const std::string kParamOutputCompressionPxr24 = "pxr24  Compression (with loss) in 24bit float";
static const std::string kParamOutputCompressionB44   = "b44    Compression (with loss) with blocs of size 4x4 pixels. Fix rate of compression.";
static const std::string kParamOutputCompressionB44a  = "b44a   Compression (with loss) with blocs of size 4x4 pixels. Non-fix rate of compression.";


enum EParamCompression
{
	eParamCompressionNone = 0,
	eParamCompressionZip,
	eParamCompressionZips,
	eParamCompressionRle,
	eParamCompressionPiz,
	eParamCompressionPxr24,
	eParamCompressionB44,
	eParamCompressionB44a
};


}
}
}
}

#endif
