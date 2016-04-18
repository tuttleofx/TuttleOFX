#ifndef _TUTTLE_PLUGIN_PDE_DENOISER_PLUGIN_FACTORY_HPP_
#define _TUTTLE_PLUGIN_PDE_DENOISER_PLUGIN_FACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace diffusion
{

static const bool kSupportTiles(true);
static const int kDefaultInterpolationValue(1);
static const double kDefaultAmplitudeValue(2.0);

mDeclarePluginFactory(AnisotropicDiffusionPluginFactory, {}, {});
}
}
}
}

#endif
