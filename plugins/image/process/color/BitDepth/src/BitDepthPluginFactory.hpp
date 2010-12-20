#ifndef _TUTTLE_PLUGIN_BITDEPTHPLUGINFACTORY_HPP_
#define _TUTTLE_PLUGIN_BITDEPTHPLUGINFACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace bitDepth {

static const bool kSupportTiles = true;

mDeclarePluginFactory( BitDepthPluginFactory, {}, {}
                       );

}
}
}

#endif
