#ifndef _OPENIMAGEIOWRITER_DEFINITIONS_HPP_
#define _OPENIMAGEIOWRITER_DEFINITIONS_HPP_

#include <tuttle/ioplugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace openImageIO
{
namespace writer
{

enum ETuttlePluginBitDepth
{
    eTuttlePluginBitDepthAuto = 0,
    eTuttlePluginBitDepth8,
    eTuttlePluginBitDepth10,
    eTuttlePluginBitDepth12,
    eTuttlePluginBitDepth16,
    eTuttlePluginBitDepth16f,
    eTuttlePluginBitDepth32,
    eTuttlePluginBitDepth32f
};

enum ETuttlePluginComponents
{
    eTuttlePluginComponentsAuto = 0,
    eTuttlePluginComponentsGray,
    eTuttlePluginComponentsRGB,
    eTuttlePluginComponentsRGBA
};

static const std::string kParamOutputQuality = "quality";
static const std::string kParamOutputQualityLabel = "Quality";

static const std::string kParamOutputOrientation = "orientation";
static const std::string kParamOutputOrientationLabel = "Orientation";

static const std::string kParamOutputOrientationNormal = "normal";
static const std::string kParamOutputOrientationFlop = "flop";
static const std::string kParamOutputOrientationR180 = "180";
static const std::string kParamOutputOrientationFlip = "flip";
static const std::string kParamOutputOrientationTransposed = "transposed";
static const std::string kParamOutputOrientationR90Clockwise = "90clockwise";
static const std::string kParamOutputOrientationTransverse = "transverse";
static const std::string kParamOutputOrientationR90CounterClockwise = "90counter-clockwise";

static const std::string kParamOutputCompression = "compression";
static const std::string kParamOutputCompressionLabel = "Compression";

static const std::string kParamOutputCompressionNone = "none   No compression";
static const std::string kParamOutputCompressionZip = "zip    Compression zlib, with blocs of 16 scanlines.";
static const std::string kParamOutputCompressionZips = "zips   Compression zlib, with blocs with only one scanline.";
static const std::string kParamOutputCompressionRle = "rle    Compression Run Length Encoding.";
static const std::string kParamOutputCompressionPiz = "piz    Piz-based wavelet compression.";
static const std::string kParamOutputCompressionPxr24 = "pxr24  Compression (with loss) in 24bit float";
static const std::string kParamOutputCompressionB44 =
    "b44    Compression (with loss) with blocs of size 4x4 pixels. Fix rate of compression.";
static const std::string kParamOutputCompressionB44a =
    "b44a   Compression (with loss) with blocs of size 4x4 pixels. Non-fix rate of compression.";

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
