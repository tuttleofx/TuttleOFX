#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 1

#include <tuttle/plugin/Plugin.hpp>
#include "CropPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{

void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::crop::CropPluginFactory, "tuttle.crop");
}
}
}
