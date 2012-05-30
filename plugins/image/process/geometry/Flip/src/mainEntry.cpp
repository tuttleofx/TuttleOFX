#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "FlipPluginFactory.hpp"

namespace OFX {
namespace Plugin {

void getPluginIDs( OFX::PluginFactoryArray& ids )
{
        mAppendPluginFactory( ids, tuttle::plugin::flip::FlipPluginFactory, "tuttle.flip" );
}

}
}
