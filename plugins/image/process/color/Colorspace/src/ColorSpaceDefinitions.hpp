#ifndef _TUTTLE_COLORSPACE_DEFINITIONS_HPP
#define _TUTTLE_COLORSPACE_DEFINITIONS_HPP

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/color/colorSpaceAPI.hpp>

namespace tuttle {
namespace plugin {
namespace colorspace {

static const bool kSupportTiles              = false;
static const bool kSupportTemporalClipAccess = false;

const static std::string kColorSpaceHelpButton = "Help";

const static std::string kColorSpaceHelpString = "<b>Help me, help me ....<br />";

const static std::string kColorSpaceIn			= "In";
const static std::string kColorSpaceOut			= "Out";

const static std::string kColorSpaceReferenceSpaceIn	= "Input Reference Space";
const static std::string kColorSpaceReferenceSpaceOut	= "Output Reference Space";

const static std::string kColorSpaceCostumizedIn	= "Input Costumized";
const static std::string kColorSpaceCostumizedOut	= "Output Costumized";

const static std::string kColorSpaceGradationLawIn	= "In Gradation Law";
const static std::string kColorSpaceGradationLawOut	= "Out Gradation Law";


const static std::string kColorSpaceInGammaValue	= "In Gamma Value";
const static std::string kColorSpaceInBlackPoint	= "In Black Point";
const static std::string kColorSpaceInWhitePoint	= "In White Point";
const static std::string kColorSpaceInGammaSensito	= "In Gamma Sensito";

const static std::string kColorSpaceOutGammaValue	= "Out Gamma Value";
const static std::string kColorSpaceOutBlackPoint	= "Out Black Point";
const static std::string kColorSpaceOutWhitePoint	= "Out White Point";
const static std::string kColorSpaceOutGammaSensito	= "Out Gamma Sensito";

const static std::string kColorSpaceLayoutIn		= "In Layout";
const static std::string kColorSpaceLayoutOut		= "Out Layout";

const static std::string kColorSpaceTempColorIn		= "In Color Temperature";
const static std::string kColorSpaceTempColorOut	= "Out Color Temperature";

const static std::string kColorSpacePrimariesIn		= "In Primaries";
const static std::string kColorSpacePrimariesOut	= "Out Primaries";

const static std::string kColorSpaceXrIn		= "In Xr";
const static std::string kColorSpaceYrIn		= "In Yr";
const static std::string kColorSpaceXgIn		= "In Xg";
const static std::string kColorSpaceYgIn		= "In Yg";
const static std::string kColorSpaceXbIn		= "In Xb";
const static std::string kColorSpaceYbIn		= "In Yb";

const static std::string kColorSpaceXrOut		= "Out Xr";
const static std::string kColorSpaceYrOut		= "Out Yr";
const static std::string kColorSpaceXgOut		= "Out Xg";
const static std::string kColorSpaceYgOut		= "Out Yg";
const static std::string kColorSpaceXbOut		= "Out Xb";
const static std::string kColorSpaceYbOut		= "Out Yb";

}
}
}

#endif
