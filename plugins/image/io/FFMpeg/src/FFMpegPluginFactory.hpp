#ifndef _TUTTLE_PLUGIN_FFMPEG_PLUGIN_FACTORY_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_PLUGIN_FACTORY_HPP_
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace reader {

static const bool   kSupportTiles                 = false;
static const bool   kSupportTemporalClipAccess    = false;

mDeclarePluginFactory( FFMpegPluginFactory, { }, { } );

}
}
}
}

#endif
