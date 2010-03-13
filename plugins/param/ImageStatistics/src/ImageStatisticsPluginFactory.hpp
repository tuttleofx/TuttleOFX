#ifndef _TUTTLE_PLUGIN_IMAGESTATISTICS_PLUGIN_FACTORY_HPP_
#define _TUTTLE_PLUGIN_IMAGESTATISTICS_PLUGIN_FACTORY_HPP_
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace average {

static const bool   kSupportTiles                 = false;
static const bool   kSupportTemporalClipAccess    = false;

mDeclarePluginFactory( ImageStatisticsPluginFactory, { }, { } );

}
}
}

#endif
