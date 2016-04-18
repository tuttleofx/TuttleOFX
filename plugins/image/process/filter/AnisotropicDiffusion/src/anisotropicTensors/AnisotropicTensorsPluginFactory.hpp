#ifndef _TUTTLE_PLUGIN_PDE_TENSORS_PLUGIN_FACTORY_HPP_
#define _TUTTLE_PLUGIN_PDE_TENSORS_PLUGIN_FACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace tensors
{

using namespace OFX;

static const bool kSupportTiles(true);
static const double kDefaultSharpnessValue(0.3);
static const double kDefaultAnisotropyValue(1.0);
static const double kDefaultAlphaValue(0.8);
static const double kDefaultSigmaValue(2.0);
static const double kDefaultGeomFactValue(1.0);
static const double kDefaultThresholdValue(0.00001);
static const int kDefaultTensorsAlgo(2);

mDeclarePluginFactory(AnisotropicTensorsPluginFactory, {}, {});
}
}
}
}

#endif // PDE_TENSORS_PLUGIN_FACTORY_HPP
