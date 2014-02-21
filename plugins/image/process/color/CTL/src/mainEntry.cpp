#define OFXPLUGIN_VERSION_MAJOR 2
#define OFXPLUGIN_VERSION_MINOR 0

#include "CTLPluginFactory.hpp"
#include <tuttle/plugin/Plugin.hpp>

namespace OFX {
namespace Plugin {

void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	mAppendPluginFactory( ids, tuttle::plugin::ctl::CTLPluginFactory, "tuttle.ctl" );
}

}
}

