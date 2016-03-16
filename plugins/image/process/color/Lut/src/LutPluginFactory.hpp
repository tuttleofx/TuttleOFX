#ifndef _TUTTLE_PLUGIN_LUTPLUGINFACTORY_HPP_
#define _TUTTLE_PLUGIN_LUTPLUGINFACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace lut
{
static const bool kSupportTiles = true;
mDeclarePluginFactory(LutPluginFactory, {}, {});
}
}
}

#endif
