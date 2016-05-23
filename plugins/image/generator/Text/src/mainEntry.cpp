#define OFXPLUGIN_VERSION_MAJOR 6
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "TextPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{

void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::text::TextPluginFactory, "tuttle.text");
}
}
}
