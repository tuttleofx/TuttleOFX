#ifndef _TUTTLE_PLUGIN_SOBEL_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_SOBEL_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


namespace tuttle {
namespace plugin {
namespace sobel {

const static std::string kParamSize = "size";
const static std::string kParamGroupAdvanced = "advanced";
const static std::string kParamUnidimensional = "unidimensional";
const static std::string kParamReverseKernel = "reverseKernel";
const static std::string kParamNormalizedKernel = "normalizedKernel";
const static std::string kParamKernelEpsilon = "kernelEpsilon";
const static std::string kParamPass = "pass";
const static std::string kParamPassFull = "full";
const static std::string kParamPass1 = "1";
const static std::string kParamPass2 = "2";

enum EParamPass
{
	eParamPassFull = 0,
	eParamPass1,
	eParamPass2
};

const static std::string kParamBorder = "border";
const static std::string kParamBorderMirror = "Mirror";
const static std::string kParamBorderConstant = "Constant";
const static std::string kParamBorderBlack = "Black";
const static std::string kParamBorderPadded = "Padded";

enum EParamBorder
{
	eParamBorderMirror = 0,
	eParamBorderConstant,
	eParamBorderBlack,
	eParamBorderPadded
};

const static std::string kParamComputeGradientNorm = "computeGradientNorm";
const static std::string kParamGradientNormManhattan = "gradientNormManhattan";
const static std::string kParamComputeGradientDirection = "computeGradientDirection";
const static std::string kParamGradientDirectionAbs = "gradientDirectionAbs";

const static std::string kParamOutputComponent = "outputComponent";
const static std::string kParamOutputComponentRGBA = "RGBA";
const static std::string kParamOutputComponentRGB = "RGB";

enum EParamOutputComponent
{
	eParamOutputComponentRGBA = 0,
	eParamOutputComponentRGB,
};

const static std::string kParamInfos = "infos";


}
}
}

#endif
