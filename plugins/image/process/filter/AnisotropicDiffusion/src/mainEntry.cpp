#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 1

#include <tuttle/plugin/Plugin.hpp>
#include "anisotropicTensors/AnisotropicTensorsPluginFactory.hpp"
#include "anisotropicDiffusion/AnisotropicDiffusionPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{

void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::anisotropicFilter::tensors::AnisotropicTensorsPluginFactory,
                         "tuttle.anisotropictensors");
    mAppendPluginFactory(ids, tuttle::plugin::anisotropicFilter::diffusion::AnisotropicDiffusionPluginFactory,
                         "tuttle.anisotropicdiffusion");
}
}
}
