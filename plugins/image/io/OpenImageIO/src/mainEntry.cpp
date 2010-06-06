#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "reader/OpenImageIOReaderPluginFactory.hpp"
#include "writer/OpenImageIOWriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	mAppendPluginFactory( ids, tuttle::plugin::openImageIO::reader::OpenImageIOReaderPluginFactory, "fr.tuttle.openimageioreader" );
	mAppendPluginFactory( ids, tuttle::plugin::openImageIO::writer::OpenImageIOWriterPluginFactory, "fr.tuttle.openimageiowriter" );
}

}
}
