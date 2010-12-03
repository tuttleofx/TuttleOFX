#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include "LocalMaximaPluginFactory.hpp"
#include <tuttle/plugin/Plugin.hpp>

namespace OFX {
namespace Plugin {

void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	mAppendPluginFactory( ids, tuttle::plugin::localmaxima::LocalMaximaPluginFactory, "fr.tuttle.duranduboi.localmaxima" );
}

}
}

