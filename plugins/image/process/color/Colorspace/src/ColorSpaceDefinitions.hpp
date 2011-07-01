#ifndef _TUTTLE_COLORSPACE_DEFINITIONS_HPP
#define _TUTTLE_COLORSPACE_DEFINITIONS_HPP

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/color/colorDefinitions.hpp>

namespace tuttle {
namespace plugin {
namespace colorspace {

static const bool kSupportTiles				= true;
static const bool kSupportTemporalClipAccess		= true;

static const std::string kColorSpaceIn			= "In";
static const std::string kColorSpaceOut			= "Out";

static const std::string kColorSpaceReferenceSpaceIn	= "Input Reference Space";
static const std::string kColorSpaceReferenceSpaceOut	= "Output Reference Space";

static const std::string kColorSpaceCustomizedIn	= "Input Customized";
static const std::string kColorSpaceCustomizedOut	= "Output Customized";

static const std::string kColorSpaceGradationLawIn	= "In Gradation Law";
static const std::string kColorSpaceGradationLawOut	= "Out Gradation Law";


static const std::string kColorSpaceInGammaValue	= "In Gamma Value";
static const std::string kColorSpaceInBlackPoint	= "In Black Point";
static const std::string kColorSpaceInWhitePoint	= "In White Point";
static const std::string kColorSpaceInGammaSensito	= "In Gamma Sensito";

static const std::string kColorSpaceOutGammaValue	= "Out Gamma Value";
static const std::string kColorSpaceOutBlackPoint	= "Out Black Point";
static const std::string kColorSpaceOutWhitePoint	= "Out White Point";
static const std::string kColorSpaceOutGammaSensito	= "Out Gamma Sensito";

static const std::string kColorSpaceLayoutIn		= "In Layout";
static const std::string kColorSpaceLayoutOut		= "Out Layout";

static const std::string kColorSpaceTempColorIn		= "In Color Temperature";
static const std::string kColorSpaceTempColorOut	= "Out Color Temperature";

static const std::string kColorSpacePrimariesIn		= "In Primaries";
static const std::string kColorSpacePrimariesOut	= "Out Primaries";

static const std::string kColorSpaceXrIn		= "In Xr";
static const std::string kColorSpaceYrIn		= "In Yr";
static const std::string kColorSpaceXgIn		= "In Xg";
static const std::string kColorSpaceYgIn		= "In Yg";
static const std::string kColorSpaceXbIn		= "In Xb";
static const std::string kColorSpaceYbIn		= "In Yb";

static const std::string kColorSpaceXrOut		= "Out Xr";
static const std::string kColorSpaceYrOut		= "Out Yr";
static const std::string kColorSpaceXgOut		= "Out Xg";
static const std::string kColorSpaceYgOut		= "Out Yg";
static const std::string kColorSpaceXbOut		= "Out Xb";
static const std::string kColorSpaceYbOut		= "Out Yb";

}
}
}

#endif
