#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "reader/DPXReaderPluginFactory.hpp"
#include "writer/DPXWriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	mAppendPluginFactory( ids, tuttle::plugin::dpx::reader::DPXReaderPluginFactory, "fr.tuttle.dpxreader" );
	mAppendPluginFactory( ids, tuttle::plugin::dpx::writer::DPXWriterPluginFactory, "fr.tuttle.dpxwrite" );
	static tuttle::plugin::dpx::reader::DPXReaderPluginFactory p1( "fr.tuttle.dpxreader", 1, 0 );
}

}
}
