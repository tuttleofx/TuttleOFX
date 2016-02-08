#ifndef _TUTTLE_PLUGIN_TURBOJPEG_WRITER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_TURBOJPEG_WRITER_DEFINITIONS_HPP_

#include <tuttle/ioplugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace turboJpeg {
namespace writer {

enum ETuttlePluginBitDepth
{
	eTuttlePluginBitDepth8
};

static const std::string kParamQuality = "quality";

static const std::string kParamSubsampling = "subsampling";
static const std::string kParamSubsamplingLabel = "Subsampling";
static const std::string kParamSubsamplingHint = "4:4:4 : one chrominance component for every pixel.\n"
				"4:2:2 : one chrominance component for every 2x1 block of pixels.\n"
				"4:2:0 : one chrominance component for every 2x2 block of pixels.\n"
				"Grayscale : no chrominance components.\n"
				"4:4:0 : one chrominance component for every 1x2 block of pixels.\n";

static const std::string kTurboJpegSubsampling444 = "444";
static const std::string kTurboJpegSubsampling422 = "422";
static const std::string kTurboJpegSubsampling420 = "420";
static const std::string kTurboJpegSubsamplingGray = "gray";
static const std::string kTurboJpegSubsampling440 = "440";

enum ETurboJpegSubsampling
{
	eTurboJpegSubsampling444 = 0,
	eTurboJpegSubsampling422,
	eTurboJpegSubsampling420,
	eTurboJpegSubsamplingGray,
	eTurboJpegSubsampling440
};

static const std::string kParamOptimization = "optimization";
static const std::string kParamOptimizationLabel = "CPU Optimization";
static const std::string kParamOptimizationHint = "Enable/disable optimizations.";

static const std::string kTurboJpegOptimizationNone = "none";
static const std::string kTurboJpegOptimizationMMX = "mmx";
static const std::string kTurboJpegOptimizationSSE = "sse";
static const std::string kTurboJpegOptimizationSSE2 = "sse2";
static const std::string kTurboJpegOptimizationSSE3 = "sse3";

enum ETurboJpegOptimization
{
	eTurboJpegOptimizationNone = 0,
	eTurboJpegOptimizationMMX,
	eTurboJpegOptimizationSSE,
	eTurboJpegOptimizationSSE2,
	eTurboJpegOptimizationSSE3
};


}
}
}
}

#endif
