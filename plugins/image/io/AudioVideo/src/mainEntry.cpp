#define OFXPLUGIN_VERSION_MAJOR 4
#define OFXPLUGIN_VERSION_MINOR 9

#include <tuttle/plugin/Plugin.hpp>
#include "reader/AVReaderPluginFactory.hpp"
#include "writer/AVWriterPluginFactory.hpp"
#include "swscale/SwscalePluginFactory.hpp"

namespace OFX
{
namespace Plugin
{

void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::av::reader::AVReaderPluginFactory, "tuttle.avreader");
    mAppendPluginFactory(ids, tuttle::plugin::av::writer::AVWriterPluginFactory, "tuttle.avwriter");
    mAppendPluginFactory(ids, tuttle::plugin::swscale::SwscalePluginFactory, "tuttle.swscale");
}
}
}
