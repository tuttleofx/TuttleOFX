/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#ifndef IFFT_PLUGIN_FACTORY_HPP
#define IFFT_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace ifft {

static const bool kSupportTiles       = false;
static const bool kHostFrameThreading = false;

mDeclarePluginFactory( IfftPluginFactory, {}, {}
                       );

}
}
}
}

#endif
