#ifndef LOG2LIN_PLUGIN_FACTORY_HPP
#define LOG2LIN_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace colorspace {
namespace log2lin {

static const bool   kSupportTiles                 = false;
static const bool   kSupportTemporalClipAccess    = false;

mDeclarePluginFactory( Log2LinPluginFactory, {}, {} );

}
}
}
}

#endif
