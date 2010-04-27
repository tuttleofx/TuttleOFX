#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "lin2log/Lin2LogPluginFactory.hpp"
#include "log2lin/Log2LinPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	mAppendPluginFactory( ids, tuttle::plugin::colorspace::lin2log::Lin2LogPluginFactory, "fr.tuttle.lin2log" );
	mAppendPluginFactory( ids, tuttle::plugin::colorspace::log2lin::Log2LinPluginFactory, "fr.tuttle.log2lin" );
}

}
}
