#define OFXPLUGIN_VERSION_MAJOR 3
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "reader/EXRReaderPluginFactory.hpp"
#include "writer/EXRWriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{
void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::exr::reader::EXRReaderPluginFactory, "tuttle.exrreader");
    mAppendPluginFactory(ids, tuttle::plugin::exr::writer::EXRWriterPluginFactory, "tuttle.exrwriter");
}
}
}
