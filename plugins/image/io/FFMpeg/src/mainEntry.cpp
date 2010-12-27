#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "reader/FFMpegReaderPluginFactory.hpp"
#include "writer/FFMpegWriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{

void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	mAppendPluginFactory( ids, tuttle::plugin::ffmpeg::reader::FFMpegReaderPluginFactory, "fr.tuttle.ffmpegreader" );
	mAppendPluginFactory( ids, tuttle::plugin::ffmpeg::writer::FFMpegWriterPluginFactory, "fr.tuttle.ffmpegwriter" );
}

}
}
