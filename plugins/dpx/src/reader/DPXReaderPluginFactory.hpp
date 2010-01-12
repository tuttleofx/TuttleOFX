#ifndef DPX_READER_PLUGIN_FACTORY_HPP
#define DPX_READER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace reader {

using namespace OFX;

static const bool kSupportTiles              = false;
static const bool kSupportTemporalClipAccess = false;

mDeclarePluginFactory( DPXReaderPluginFactory, {}, {} );

}
}
}
}

#endif
