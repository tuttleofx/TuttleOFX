#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>

#include "OCIOColorSpacePluginFactory.hpp"


namespace OFX {
namespace Plugin {

void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	mAppendPluginFactory( ids, tuttle::plugin::ocio::colorspace::OCIOColorSpacePluginFactory, "tuttle.ocio.colorspace" );
}

}
}
