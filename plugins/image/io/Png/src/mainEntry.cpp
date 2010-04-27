#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "reader/PNGReaderPluginFactory.hpp"
#include "writer/PNGWriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	mAppendPluginFactory( ids, tuttle::plugin::png::reader::PNGReaderPluginFactory, "fr.tuttle.pngreader" );
	mAppendPluginFactory( ids, tuttle::plugin::png::writer::PNGWriterPluginFactory, "fr.tuttle.pngwriter" );
}

}
}
