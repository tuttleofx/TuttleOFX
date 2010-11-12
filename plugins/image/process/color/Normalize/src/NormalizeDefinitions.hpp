#ifndef _TUTTLE_PLUGIN_NORMALIZE_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_NORMALIZE_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>

namespace tuttle {
namespace plugin {
namespace normalize {

const static std::string kParamHelpButton = "Help";

const static std::string kParamMode = "mode";
const static std::string kParamModeAnalyse = "analyse";
const static std::string kParamModeCustom = "custom";
enum EParamMode
{
	eParamModeAnalyse = 0,
	eParamModeCustom
};
const static std::string kParamAnalyseNow = "analyseNow";

const static std::string kParamAnalyseMode = "analyseMode";
const static std::string kParamAnalysePerChannel = "perChannel";
const static std::string kParamAnalyseLuminosity = "luminosity";
const static std::string kParamAnalyseR = "r";
const static std::string kParamAnalyseG = "g";
const static std::string kParamAnalyseB = "b";
const static std::string kParamAnalyseA = "a";
enum EParamAnalyseMode
{
	eParamAnalyseModePerChannel = 0,
	eParamAnalyseModeLuminosity,
	eParamAnalyseModeR,
	eParamAnalyseModeG,
	eParamAnalyseModeB,
	eParamAnalyseModeA
};

const static std::string kParamSrcGroup = "srcGroup";
const static std::string kParamSrcCustomColorMin = "srcColorMin";
const static std::string kParamSrcCustomColorMax = "srcColorMax";
const static std::string kParamSrcCustomValueMin = "srcValueMin";
const static std::string kParamSrcCustomValueMax = "srcValueMax";

const static std::string kParamDstGroup = "dstGroup";
const static std::string kParamDstCustomColorMin = "dstColorMin";
const static std::string kParamDstCustomColorMax = "dstColorMax";
const static std::string kParamDstCustomValueMin = "dstValueMin";
const static std::string kParamDstCustomValueMax = "dstValueMax";

const static std::string kParamProcessGroup = "processGroup";
const static std::string kParamProcessR = "processR";
const static std::string kParamProcessG = "processG";
const static std::string kParamProcessB = "processB";
const static std::string kParamProcessA = "processA";


}
}
}

#endif
