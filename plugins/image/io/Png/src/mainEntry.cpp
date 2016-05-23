#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "reader/PngReaderPluginFactory.hpp"
#include "writer/PngWriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::png::reader::PngReaderPluginFactory, "tuttle.pngreader");
    mAppendPluginFactory(ids, tuttle::plugin::png::writer::PngWriterPluginFactory, "tuttle.pngwriter");
}
}
}
