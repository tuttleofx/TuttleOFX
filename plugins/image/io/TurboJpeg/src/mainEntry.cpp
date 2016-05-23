#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include <tuttle/plugin/Plugin.hpp>
#include "reader/TurboJpegReaderPluginFactory.hpp"
#include "writer/TurboJpegWriterPluginFactory.hpp"

namespace OFX
{
namespace Plugin
{

void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::turboJpeg::reader::TurboJpegReaderPluginFactory, "tuttle.turbojpegreader");
    mAppendPluginFactory(ids, tuttle::plugin::turboJpeg::writer::TurboJpegWriterPluginFactory, "tuttle.turbojpegwriter");
}
}
}
