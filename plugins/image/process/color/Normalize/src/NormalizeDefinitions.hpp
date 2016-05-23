#ifndef _TUTTLE_PLUGIN_NORMALIZE_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_NORMALIZE_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace normalize
{

static const std::string kParamHelpButton = "Help";

static const std::string kParamMode = "mode";
static const std::string kParamModeAnalyse = "analyse";
static const std::string kParamModeCustom = "custom";
enum EParamMode
{
    eParamModeAnalyse = 0,
    eParamModeCustom
};
static const std::string kParamAnalyseNow = "analyseNow";

static const std::string kParamAnalyseMode = "analyseMode";
static const std::string kParamAnalysePerChannel = "perChannel";
static const std::string kParamAnalyseLuminosity = "luminosity";
static const std::string kParamAnalyseR = "r";
static const std::string kParamAnalyseG = "g";
static const std::string kParamAnalyseB = "b";
static const std::string kParamAnalyseA = "a";
enum EParamAnalyseMode
{
    eParamAnalyseModePerChannel = 0,
    eParamAnalyseModeLuminosity,
    eParamAnalyseModeR,
    eParamAnalyseModeG,
    eParamAnalyseModeB,
    eParamAnalyseModeA
};

static const std::string kParamSrcGroup = "srcGroup";
static const std::string kParamSrcCustomColorMin = "srcColorMin";
static const std::string kParamSrcCustomColorMax = "srcColorMax";
static const std::string kParamSrcCustomValueMin = "srcValueMin";
static const std::string kParamSrcCustomValueMax = "srcValueMax";

static const std::string kParamDstGroup = "dstGroup";
static const std::string kParamDstCustomColorMin = "dstColorMin";
static const std::string kParamDstCustomColorMax = "dstColorMax";
static const std::string kParamDstCustomValueMin = "dstValueMin";
static const std::string kParamDstCustomValueMax = "dstValueMax";

static const std::string kParamProcessGroup = "processGroup";
static const std::string kParamProcessR = "processR";
static const std::string kParamProcessG = "processG";
static const std::string kParamProcessB = "processB";
static const std::string kParamProcessA = "processA";
}
}
}

#endif
