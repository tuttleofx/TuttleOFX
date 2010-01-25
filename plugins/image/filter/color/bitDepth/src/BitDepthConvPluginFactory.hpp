#ifndef BITDEPTHCONV_PLUGIN_FACTORY_H
#define BITDEPTHCONV_PLUGIN_FACTORY_H
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace bitDepthConvert {

static const bool   kSupportTiles                 = false;
static const bool   kSupportTemporalClipAccess    = false;

mDeclarePluginFactory( BitDepthConvPluginFactory, { }, { } );

}
}
}

#endif //BITDEPTHCONV_PLUGIN_FACTORY_H
