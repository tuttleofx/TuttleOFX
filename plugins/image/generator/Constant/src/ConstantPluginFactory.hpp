#ifndef _TUTTLE_PLUGIN_CONSTANT_PLUGIN_FACTORY_HPP_
#define _TUTTLE_PLUGIN_CONSTANT_PLUGIN_FACTORY_HPP_
#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace constant {

static const bool kSupportTiles = false;

mDeclarePluginFactory( ConstantPluginFactory, {}, {}
                       );

}
}
}

#endif
