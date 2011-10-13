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
/*
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
*/
static const std::string kParamPCVideo        = "PC-Video            640x480";
static const std::string kParamNTSC           = "NTSC                720x486";
static const std::string kParamPAL            = "PAL                 720x576";
static const std::string kParamHD             = "HD                  1920x1080";
static const std::string kParamNTSC169        = "NTSC-16:9           720x486";
static const std::string kParamPAL169         = "PAL-16:9            720x576";
static const std::string kParam1kSuper35      = "1K-Super35-full-ap  1024x778";
static const std::string kParam1kCinemascope  = "1K-Cinemascope      914x778";
static const std::string kParam2kSuper35      = "2K-Super35-full-ap  2048x1556";
static const std::string kParam2kCinemascope  = "2K-Cinemascope      1828x1556";
static const std::string kParam4kSuper35      = "4K-Super35-full-ap  4096x3112";
static const std::string kParam4kCinemascope  = "4K-Cinemascope      3656x3112";
static const std::string kParamSquare256      = "Square-256          256x256";
static const std::string kParamSquare512      = "Square-512          512x512";
static const std::string kParamSquare1k       = "Square-1k           1024x1024";
static const std::string kParamSquare2k       = "Square-2k           2048x2048";

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
static const std::string kParamFilterSigma     = "sigma";

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


static const std::string kParamFilterOutOfImage      = "outOfImage";

static const std::string kParamFilterOutBlack        = "black";
static const std::string kParamFilterOutTransparency = "transparency";
static const std::string kParamFilterOutCopy         = "copy";
static const std::string kParamFilterOutMirror       = "mirror";

enum EParamFilterOutOfImage
{
    eParamFilterOutBlack = 0,
    eParamFilterOutTransparency,
    eParamFilterOutCopy,
    eParamFilterOutMirror
};

}
}
}

#endif
