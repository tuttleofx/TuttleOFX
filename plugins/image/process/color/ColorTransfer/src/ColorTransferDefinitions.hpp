#ifndef _TUTTLE_PLUGIN_COLORTRANSFER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_COLORTRANSFER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorTransfer
{

static const std::string kClipSrcRef("srcRef");
static const std::string kClipDstRef("dstRef");

static const std::string kParamColorspace("colorspace");
static const std::string kParamColorspaceNone("none");
static const std::string kParamColorspaceLMS("LMS");
static const std::string kParamColorspaceLab("LAlphaBeta");

enum EColorspace
{
    eColorspaceNone = 0,
    eColorspaceLMS,
    eColorspaceLab
};

static const std::string kParamAverageCoef("averageCoef");
static const std::string kParamDynamicCoef("dynamicCoef");

static const std::string kParamRegionA("regionA");
static const std::string kParamRegionB("regionB");
static const std::string kParamSameRegion("sameRegion");
static const std::string kParamInputRegionA("inputRegionA");
static const std::string kParamInputRegionB("inputRegionB");
}
}
}

#endif
