#ifndef _TUTTLE_PLUGIN_IDKEYER_PLUGIN_FACTORY_HPP_
#define _TUTTLE_PLUGIN_IDKEYER_PLUGIN_FACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace idKeyer
{

static const bool kSupportTiles = true;

mDeclarePluginFactory(IdKeyerPluginFactory, {}, {});
}
}
}

#endif
