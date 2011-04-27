#ifndef _TUTTLE_PLUGIN_RESIZE_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_RESIZE_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


namespace tuttle {
namespace plugin {
namespace resize {

static const std::string kParamOptions		= "Options";
static const std::string kParamFormat		= "To format";
static const std::string kParamBox		= "To box";
static const std::string kParamScale		= "To scale";
enum EParamOptions
{
	eParamFormat = 0,
	eParamBox,
	eParamScale
};

static const std::string kParamFormatDim	= "format";
static const std::string kParamPCVideo		= " 640 x  480  PC Video";
static const std::string kParamNTSC		= " 720 x  486  NTSC";
static const std::string kParamPAL		= " 720 x  576  PAL";
static const std::string kParamHD		= "1920 x 1080  HD";
static const std::string kParamNTSC169		= " 720 x  486  NTSC 16:9";
static const std::string kParamPAL169		= " 720 x  576  PAL  16:9";
static const std::string kParam1kSuper35	= "1024 x  778  1K Super 35 (full-ap)";
static const std::string kParam1kCinemascope	= " 914 x  778  1K Cinemascope";
static const std::string kParam2kSuper35	= "2048 x 1556  2K Super 35 (full-ap)";
static const std::string kParam2kCinemascope	= "1828 x 1556  2K Cinemascope";
static const std::string kParam4kSuper35	= "4096 x 3112  4K Super 35 (full-ap)";
static const std::string kParam4kCinemascope	= "3656 x 3112  4K Cinemascope";
static const std::string kParamSquare256	= " 256 x  256  Square 256";
static const std::string kParamSquare512	= " 512 x  512  Square 512";
static const std::string kParamSquare1k		= "1024 x 1024  Square 1k";
static const std::string kParamSquare2k		= "2048 x 2048  Square 2k";

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


static const std::string kParamOutputFormat	= "Output Format";
static const std::string kParamOutputDimension	= "Output Dimension";

static const std::string kParamDirection	= "Direction";
static const std::string kParamSizeX		= "Width";
static const std::string kParamSizeY		= "Height";

enum EParamSize
{
	eParamSizeX = 0,
	eParamSizeY
};

static const std::string kParamScaleDim		= "Scale";

static const std::string kParamScaleX		= "ScaleX";
static const std::string kParamScaleY		= "ScaleY";

static const std::string kParamSplit		= "Split";

static const std::string kParamCenter		= "Center";
static const std::string kParamCenterPoint	= "Center Point";

static const std::string kParamFilter		= "Filter";

static const std::string kParamFilterNearest	= "Nearest neighbor";
static const std::string kParamFilterBilinear	= "Bilinear";
static const std::string kParamFilterBicubic	= "Bicubic";
static const std::string kParamFilterCatmul	= "Catmul-Rom";
static const std::string kParamFilterKeys	= "Keys";
static const std::string kParamFilterSimon	= "Simon";
static const std::string kParamFilterRifman	= "Rifman";
static const std::string kParamFilterLanczos3	= "Lanczos 3";
static const std::string kParamFilterLanczos4	= "Lanczos 4";
static const std::string kParamFilterLanczos6	= "Lanczos 6";
static const std::string kParamFilterLanczos12	= "Lanczos 12";
static const std::string kParamFilterGaussian	= "Gaussian";
enum EParamFilter
{
	eParamFilterNearest = 0,
	eParamFilterBilinear,
	eParamFilterBicubic,
	eParamFilterCatmul,
	eParamFilterKeys,
	eParamFilterSimon,
	eParamFilterRifman,
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
