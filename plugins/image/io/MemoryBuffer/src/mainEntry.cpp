#define OFXPLUGIN_VERSION_MAJOR 1
#define OFXPLUGIN_VERSION_MINOR 0

#include "inputBuffer/InputBufferPluginFactory.hpp"
#include "outputBuffer/OutputBufferPluginFactory.hpp"
#include <tuttle/plugin/Plugin.hpp>

namespace OFX
{
namespace Plugin
{

void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::inputBuffer::InputBufferPluginFactory, "tuttle.inputbuffer");
    mAppendPluginFactory(ids, tuttle::plugin::outputBuffer::OutputBufferPluginFactory, "tuttle.outputbuffer");
}
}
}
