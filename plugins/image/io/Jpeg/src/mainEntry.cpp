#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "reader/JpegReaderPluginFactory.hpp"
#include "writer/JpegWriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{

void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::jpeg::reader::JpegReaderPluginFactory, "tuttle.jpegreader");
    mAppendPluginFactory(ids, tuttle::plugin::jpeg::writer::JpegWriterPluginFactory, "tuttle.jpegwriter");
}
}
}
