#ifndef _TUTTLE_PLUGIN_TEXTPLUGINFACTORY_HPP_
#define _TUTTLE_PLUGIN_TEXTPLUGINFACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace text {

static const bool kSupportTiles = true;

mDeclarePluginFactory( TextPluginFactory, {}, {}
                       );

}
}
}

#endif
