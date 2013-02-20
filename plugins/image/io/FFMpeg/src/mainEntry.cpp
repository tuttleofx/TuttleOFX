#define OFXPLUGIN_VERSION_MAJOR 2
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "reader/FFMpegReaderPluginFactory.hpp"
#include "writer/FFMpegWriterPluginFactory.hpp"
#include "swscale/SwscalePluginFactory.hpp"

namespace OFX {
namespace Plugin {

void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	mAppendPluginFactory( ids, tuttle::plugin::ffmpeg::reader::FFMpegReaderPluginFactory, "tuttle.ffmpegreader" );
	mAppendPluginFactory( ids, tuttle::plugin::ffmpeg::writer::FFMpegWriterPluginFactory, "tuttle.ffmpegwriter" );
	mAppendPluginFactory( ids, tuttle::plugin::swscale::SwscalePluginFactory, "tuttle.swscale" );
}

}
}
