#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "reader/ImageMagickReaderPluginFactory.hpp"
#include "writer/ImageMagickWriterPluginFactory.hpp"

namespace OFX {
namespace Plugin {

void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	mAppendPluginFactory( ids, tuttle::plugin::imagemagick::reader::ImageMagickReaderPluginFactory, "fr.tuttle.imagemagickreader" );
	mAppendPluginFactory( ids, tuttle::plugin::imagemagick::writer::ImageMagickWriterPluginFactory, "fr.tuttle.imagemagickwriter" );
}

}
}
