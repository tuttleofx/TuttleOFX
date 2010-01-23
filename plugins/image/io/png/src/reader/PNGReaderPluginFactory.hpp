#ifndef PNG_READER_PLUGIN_FACTORY_HPP
#define PNG_READER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace png {
namespace reader {

using namespace OFX;

static const bool kSupportTiles = false;

mDeclarePluginFactory( PNGReaderPluginFactory, {}, {} );

}
}
}
}

#endif
