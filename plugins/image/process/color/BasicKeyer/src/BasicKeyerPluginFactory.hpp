#ifndef _TUTTLE_PLUGINBASICKEYERPLUGINFACTORY_HPP_
#define _TUTTLE_PLUGINBASICKEYERPLUGINFACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace basicKeyer
{

static const bool kSupportTiles = true;
mDeclarePluginFactory(BasicKeyerPluginFactory, {}, {});
}
}
}

#endif
