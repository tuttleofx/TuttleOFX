#define OFXPLUGIN_VERSION_MAJOR 2
#define OFXPLUGIN_VERSION_MINOR 0

#include "ColorBarsPluginFactory.hpp"
#include <tuttle/plugin/Plugin.hpp>

namespace OFX
{
namespace Plugin
{

void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::colorBars::ColorBarsPluginFactory, "tuttle.colorbars");
}
}
}
