#ifndef _TUTTLE_PLUGIN_INVERTPLUGINFACTORY_HPP_
#define _TUTTLE_PLUGIN_INVERTPLUGINFACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace invert
{
static const bool kSupportTiles = true;
mDeclarePluginFactory(InvertPluginFactory, {}, {});
}
}
}

#endif
