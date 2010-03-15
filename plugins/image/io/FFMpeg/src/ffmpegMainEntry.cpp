#include <ofxsImageEffect.h>
#include "reader/FFMpegReaderPluginFactory.hpp"
#include "writer/FFMpegWriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static tuttle::plugin::ffmpeg::reader::FFMpegReaderPluginFactory p1( "fr.hd3d.tuttle.ffmpegreader", 1, 0 );
	ids.push_back( &p1 );
	static tuttle::plugin::ffmpeg::writer::FFMpegWriterPluginFactory p2( "fr.hd3d.tuttle.ffmpegwriter", 1, 0 );
	ids.push_back( &p2 );
}

}
}
