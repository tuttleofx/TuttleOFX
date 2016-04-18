#define OFXPLUGIN_VERSION_MAJOR 2
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "CheckerboardPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{

void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::checkerboard::CheckerboardPluginFactory, "tuttle.checkerboard");
}
}
}
