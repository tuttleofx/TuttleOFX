#ifndef _TUTTLE_PLUGIN_IMAGESTATISTICSPLUGINFACTORY_HPP_
#define _TUTTLE_PLUGIN_IMAGESTATISTICSPLUGINFACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace imageStatistics
{

static const bool kSupportTiles = false;
mDeclarePluginFactory(ImageStatisticsPluginFactory, {}, {});
}
}
}

#endif
