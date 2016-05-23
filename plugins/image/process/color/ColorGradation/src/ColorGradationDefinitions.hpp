#ifndef _TUTTLE_PLUGIN_COLORGRADATION_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_COLORGRADATION_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorGradation
{

const std::string kParamIn = "in";
const std::string kParamOut = "out";

const std::string kParamGradation_linear = "Linear";
const std::string kParamGradation_sRGB = "sRGB";
const std::string kParamGradation_Rec709 = "Rec709";
const std::string kParamGradation_cineon = "Cineon";
const std::string kParamGradation_gamma = "Gamma";
const std::string kParamGradation_panalog = "Panalog";
const std::string kParamGradation_REDLog = "REDLog";
const std::string kParamGradation_ViperLog = "ViperLog";
const std::string kParamGradation_REDSpace = "REDSpace";
const std::string kParamGradation_AlexaV3LogC = "AlexaV3LogC";

// const std::string kParamGradation_rec601  = "rec601";

enum EParamGradation
{
    eParamGradation_linear = 0,
    eParamGradation_sRGB,
    eParamGradation_Rec709,
    eParamGradation_cineon,
    eParamGradation_gamma,
    eParamGradation_panalog,
    eParamGradation_REDLog,
    eParamGradation_ViperLog,
    eParamGradation_REDSpace,
    eParamGradation_AlexaV3LogC
    //	eParamGradation_rec601
};

static const std::string kColorSpaceInGammaValue = "inGammaValue";
static const std::string kColorSpaceInBlackPoint = "inBlackPoint";
static const std::string kColorSpaceInWhitePoint = "inWhitePoint";
static const std::string kColorSpaceInGammaSensito = "inGammaSensito";

static const double kColorSpaceInBlackPointDefaultValue = 95;
static const double kColorSpaceInWhitePointDefaultValue = 685;
static const double kColorSpaceInGammaSensitoDefaultValue = 0.6;

static const std::string kColorSpaceOutGammaValue = "outGammaValue";
static const std::string kColorSpaceOutBlackPoint = "outBlackPoint";
static const std::string kColorSpaceOutWhitePoint = "outWhitePoint";
static const std::string kColorSpaceOutGammaSensito = "outGammaSensito";

const std::string kParamProcessAlpha = "processAlpha";
const std::string kParamInvert = "invert";
}
}
}

#endif
