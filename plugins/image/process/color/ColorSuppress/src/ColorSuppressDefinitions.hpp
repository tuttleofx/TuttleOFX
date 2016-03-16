#ifndef _TUTTLE_PLUGIN_COLORSUPPRESS_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_COLORSUPPRESS_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorSuppress
{

const static std::string kParamRedSuppressRate = "red";
const static std::string kParamGreenSuppressRate = "green";
const static std::string kParamBlueSuppressRate = "blue";
const static std::string kParamCyanSuppressRate = "cyan";
const static std::string kParamMagentaSuppressRate = "magenta";
const static std::string kParamYellowSuppressRate = "yellow";

const static std::string kParamApplyOn = "applyOn";
const static std::string kParamApplyOnImage = "image";
const static std::string kParamApplyOnAlpha = "alpha";
const static std::string kParamApplyOnImageAndAlpha = "image and alpha";

const static std::string kParamPreserveLuma = "preserveLuma";

const static std::string kParamObeyAlpha = "obeyAlpha";

enum EOutputType
{
    eOutputTypeImage = 0,
    eOutputTypeAlpha = 1,
    eOutputTypeAlphaImage = 2
};
}
}
}

#endif
