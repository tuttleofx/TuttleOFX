#ifndef _TUTTLE_PLUGIN_CONVOLUTIONPLUGINFACTORY_HPP_
#define _TUTTLE_PLUGIN_CONVOLUTIONPLUGINFACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace convolution
{
static const bool kSupportTiles = false;
mDeclarePluginFactory(ConvolutionPluginFactory, {}, {});
}
}
}

#endif
