#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_PLUGIN_FACTORY_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_PLUGIN_FACTORY_HPP_
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace writer {

static const bool kSupportTiles = false;

mDeclarePluginFactory( Jpeg2000WriterPluginFactory, { }, { } );

}
}
}
}

#endif
