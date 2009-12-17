#ifndef DPX_WRITER_PLUGIN_FACTORY_HPP
#define DPX_WRITER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

static const bool kSupportTiles = false;
static const bool kSupportTemporalClipAccess = false;


using namespace OFX;

mDeclarePluginFactory( DPXWriterPluginFactory, {}, {} );

}
}
}
}

#endif //DPX_WRITER_PLUGIN_FACTORY_HPP
