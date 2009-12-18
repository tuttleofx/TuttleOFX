#ifndef EXR_READER_PLUGIN_FACTORY_HPP
#define EXR_READER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

static const bool kSupportTiles = false;
static const bool kSupportTemporalClipAccess = false;


using namespace OFX;

mDeclarePluginFactory( EXRReaderPluginFactory, {}, {} );

}
}
}
}

#endif //EXR_READER_PLUGIN_FACTORY_HPP
