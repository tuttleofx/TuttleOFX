#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 5

#include <tuttle/plugin/Plugin.hpp>

#include "reader/Jpeg2000ReaderPluginFactory.hpp"
#include "writer/Jpeg2000WriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{

void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::jpeg2000::reader::Jpeg2000ReaderPluginFactory, "tuttle.jpeg2000reader");
    mAppendPluginFactory(ids, tuttle::plugin::jpeg2000::writer::Jpeg2000WriterPluginFactory, "tuttle.jpeg2000writer");
}
}
}
