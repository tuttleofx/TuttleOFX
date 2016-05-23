#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include "ColorGradationPluginFactory.hpp"
#include <tuttle/plugin/Plugin.hpp>

namespace OFX
{
namespace Plugin
{

void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::colorGradation::ColorGradationPluginFactory, "tuttle.colorgradation");
}
}
}
