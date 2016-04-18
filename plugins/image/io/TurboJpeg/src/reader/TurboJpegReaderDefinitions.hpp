#ifndef _TUTTLE_PLUGIN_TURBOJPEG_READER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_TURBOJPEG_READER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace turboJpeg
{
namespace reader
{

static const std::string kParamFastUpsampling = "fastupsampling";
static const std::string kParamFastUpsamplingLabel = "Fast Upsampling";
static const std::string kParamFastUpsamplingHint = "Fast upsampling U and V channels. Can introduce distorsions.";

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

static const std::string kParamOptimization = "optimization";
static const std::string kParamOptimizationLabel = "CPU Optimization";
static const std::string kParamOptimizationHint = "Enable/disable optimizations.";
}
}
}
}

#endif
