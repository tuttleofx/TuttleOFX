#ifndef _TUTTLE_COLORSPACE_DEFINITIONS_HPP
#define _TUTTLE_COLORSPACE_DEFINITIONS_HPP

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/color/colorDefinitions.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorspace
{

static const bool kSupportTiles = true;
static const bool kSupportTemporalClipAccess = true;

static const std::string kColorSpaceIn = "in";
static const std::string kColorSpaceOut = "out";

static const std::string kColorSpaceReferenceSpaceIn = "inputReferenceSpace";
static const std::string kColorSpaceReferenceSpaceOut = "outputReferenceSpace";

static const std::string kColorSpaceCustomizedIn = "inputCustomized";
static const std::string kColorSpaceCustomizedOut = "outputCustomized";

static const std::string kColorSpaceGradationLawIn = "inGradationLaw";
static const std::string kColorSpaceGradationLawOut = "outGradationLaw";

static const std::string kColorSpaceInGammaValue = "inGammaValue";
static const std::string kColorSpaceInBlackPoint = "inBlackPoint";
static const std::string kColorSpaceInWhitePoint = "inWhitePoint";
static const std::string kColorSpaceInGammaSensito = "inGammaSensito";

static const std::string kColorSpaceOutGammaValue = "outGammaValue";
static const std::string kColorSpaceOutBlackPoint = "outBlackPoint";
static const std::string kColorSpaceOutWhitePoint = "outWhitePoint";
static const std::string kColorSpaceOutGammaSensito = "outGammaSensito";

static const std::string kColorSpaceLayoutIn = "inLayout";
static const std::string kColorSpaceLayoutOut = "outLayout";

static const std::string kColorSpaceTempColorIn = "inColorTemperature";
static const std::string kColorSpaceTempColorOut = "outColorTemperature";

static const std::string kColorSpacePrimariesIn = "inPrimaries";
static const std::string kColorSpacePrimariesOut = "outPrimaries";

static const std::string kColorSpaceXrIn = "inXr";
static const std::string kColorSpaceYrIn = "inYr";
static const std::string kColorSpaceXgIn = "inXg";
static const std::string kColorSpaceYgIn = "inYg";
static const std::string kColorSpaceXbIn = "inXb";
static const std::string kColorSpaceYbIn = "inYb";

static const std::string kColorSpaceXrOut = "outXr";
static const std::string kColorSpaceYrOut = "outYr";
static const std::string kColorSpaceXgOut = "outXg";
static const std::string kColorSpaceYgOut = "outYg";
static const std::string kColorSpaceXbOut = "outXb";
static const std::string kColorSpaceYbOut = "outYb";
}
}
}

#endif
