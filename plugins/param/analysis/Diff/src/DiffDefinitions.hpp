#ifndef _TUTTLE_PLUGIN_DIFF_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_DIFF_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace quality
{

static const std::string kDiffSourceA = "SourceA";
static const std::string kDiffSourceB = "SourceB";

static const std::string kMeasureFunction = "measureFunction";
static const std::string kMeasureFunctionLabel = "Quality mesure function";

static const std::string kMeasureFunctionPSNR = "psnr (Peak Signal to Noise Ratio)";
static const std::string kMeasureFunctionMSE = "mse (Mean Square Error)";
static const std::string kMeasureFunctionSSIM = "ssim (Structural SIMilarity)";

enum EMeasureFunction
{
    eMeasureFunctionMSE = 0,
    eMeasureFunctionPSNR,
    eMeasureFunctionSSIM
};

static const std::string kOutputQualityMesure = "quality";
static const std::string kOutputQualityMesureLabel = "Quality";
}
}
}

#endif
