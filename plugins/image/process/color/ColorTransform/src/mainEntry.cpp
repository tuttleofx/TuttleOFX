#define OFXPLUGIN_VERSION_MAJOR 0
#define OFXPLUGIN_VERSION_MINOR 0

#include "ColorTransformPluginFactory.hpp"
#include <tuttle/plugin/Plugin.hpp>

namespace OFX
{
namespace Plugin
{

void getPluginIDs(OFX::PluginFactoryArray& ids)
{
    mAppendPluginFactory(ids, tuttle::plugin::colorTransform::ColorTransformPluginFactory, "tuttle.colortransform");
}
}
}
