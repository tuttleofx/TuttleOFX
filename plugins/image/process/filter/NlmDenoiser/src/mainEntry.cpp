#define OFXPLUGIN_VERSION_MAJOR 2
#define OFXPLUGIN_VERSION_MINOR 3

#include <tuttle/plugin/Plugin.hpp>
#include "NLMDenoiserPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{

void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::nlmDenoiser::NLMDenoiserPluginFactory, "tuttle.nlmdenoiser");
}
}
}
