/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#ifndef DPX_READER_PLUGIN_FACTORY_HPP
#define DPX_READER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace fft {

static const bool kSupportTiles       = false;
static const bool kHostFrameThreading = false;

mDeclarePluginFactory( FftPluginFactory, {}, {}
                       );

}
}
}
}

#endif
