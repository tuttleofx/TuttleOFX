#ifndef _TUTTLE_PLUGIN_RESIZE_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_RESIZE_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


namespace tuttle {
namespace plugin {
namespace resize {

static const std::string kParamOptions        = "options";
static const std::string kParamFormat         = "toFormat";
static const std::string kParamBox            = "toBox";
static const std::string kParamScale          = "toScale";
enum EParamOptions
{
	eParamFormat = 0,
	eParamBox,
	eParamScale
};

static const std::string kParamFormatDim      = "format";
static const std::string kParamPCVideo        = "640x480";
static const std::string kParamNTSC           = "720x486";
static const std::string kParamPAL            = "720x576";
static const std::string kParamHD             = "1920x1080";
static const std::string kParamNTSC169        = "720x486";
static const std::string kParamPAL169         = "720x576";
static const std::string kParam1kSuper35      = "1024x778";
static const std::string kParam1kCinemascope  = "914x778";
static const std::string kParam2kSuper35      = "2048x1556";
static const std::string kParam2kCinemascope  = "1828x1556";
static const std::string kParam4kSuper35      = "4096x3112";
static const std::string kParam4kCinemascope  = "3656x3112";
static const std::string kParamSquare256      = "256x256";
static const std::string kParamSquare512      = "512x512";
static const std::string kParamSquare1k       = "1024x1024";
static const std::string kParamSquare2k       = "2048x2048";
/*
static const std::string kParamPCVideo        = "640 x  480  PC Video";
static const std::string kParamNTSC           = "720 x  486  NTSC";
static const std::string kParamPAL            = "720 x  576  PAL";
static const std::string kParamHD             = "1920 x 1080  HD";
static const std::string kParamNTSC169        = "720 x  486  NTSC 16:9";
static const std::string kParamPAL169         = "720 x  576  PAL  16:9";
static const std::string kParam1kSuper35      = "1024 x  778  1K Super 35 (full-ap)";
static const std::string kParam1kCinemascope  = "914 x  778  1K Cinemascope";
static const std::string kParam2kSuper35      = "2048 x 1556  2K Super 35 (full-ap)";
static const std::string kParam2kCinemascope  = "1828 x 1556  2K Cinemascope";
static const std::string kParam4kSuper35      = "4096 x 3112  4K Super 35 (full-ap)";
static const std::string kParam4kCinemascope  = "3656 x 3112  4K Cinemascope";
static const std::string kParamSquare256      = "256 x  256  Square 256";
static const std::string kParamSquare512      = "512 x  512  Square 512";
static const std::string kParamSquare1k       = "1024 x 1024  Square 1k";
static const std::string kParamSquare2k       = "2048 x 2048  Square 2k";
*/
enum EParamFormat
{
	eParamPCVideo,
	eParamNTSC,
	eParamPAL,
	eParamHD,
	eParamNTSC169,
	eParamPAL169,
	eParam1kSuper35,
	eParam1kCinemascope,
	eParam2kSuper35,
	eParam2kCinemascope,
	eParam4kSuper35,
	eParam4kCinemascope,
	eParamSquare256,
	eParamSquare512,
	eParamSquare1k,
	eParamSquare2k
};


static const std::string kParamOutputFormat    = "outputFormat";
static const std::string kParamOutputDimension = "outputDimension";

static const std::string kParamDirection       = "direction";
static const std::string kParamSizeX           = "width";
static const std::string kParamSizeY           = "height";

enum EParamSize
{
	eParamSizeX = 0,
	eParamSizeY
};

static const std::string kParamScaleDim        = "scale";

static const std::string kParamScaleX          = "scaleX";
static const std::string kParamScaleY          = "scaleY";

static const std::string kParamSplit           = "split";

static const std::string kParamCenter          = "center";
static const std::string kParamCenterPoint     = "centerPoint";

static const std::string kParamFilter          = "filter";

static const std::string kParamFilterNearest   = "nearestNeighbor";
static const std::string kParamFilterBilinear  = "bilinear";
static const std::string kParamFilterBC        = "bc";
static const std::string kParamFilterBicubic   = "bicubic";
static const std::string kParamFilterCatrom    = "catrom";
static const std::string kParamFilterKeys      = "keys";
static const std::string kParamFilterSimon     = "simon";
static const std::string kParamFilterRifman    = "rifman";
static const std::string kParamFilterMitchell  = "mitchell";
static const std::string kParamFilterParzen    = "parzen";
static const std::string kParamFilterLanczos   = "lanczos";
static const std::string kParamFilterLanczos3  = "lanczos3";
static const std::string kParamFilterLanczos4  = "lanczos4";
static const std::string kParamFilterLanczos6  = "lanczos6";
static const std::string kParamFilterLanczos12 = "lanczos12";
static const std::string kParamFilterGaussian  = "gaussian";

static const std::string kParamFilterB         = "b";
static const std::string kParamFilterC         = "c";

static const std::string kParamFilterSize      = "size";

enum EParamFilter
{
	eParamFilterNearest = 0,
	eParamFilterBilinear,
	eParamFilterBC,
	eParamFilterBicubic,
	eParamFilterCatrom,
	eParamFilterKeys,
	eParamFilterSimon,
	eParamFilterRifman,
	eParamFilterMitchell,
	eParamFilterParzen,
	eParamFilterLanczos,
	eParamFilterLanczos3,
	eParamFilterLanczos4,
	eParamFilterLanczos6,
	eParamFilterLanczos12,
	eParamFilterGaussian
};

}
}
}

#endif
