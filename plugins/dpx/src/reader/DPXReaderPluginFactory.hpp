#ifndef DPX_READER_PLUGIN_FACTORY_HPP
#define DPX_READER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace reader {

static const bool kSupportTiles = false;
static const bool kSupportTemporalClipAccess = false;


using namespace OFX;

mDeclarePluginFactory( DPXReaderPluginFactory, {}, {} );

}
}
}
}

#endif //DPX_READER_PLUGIN_FACTORY_HPP
