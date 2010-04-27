#ifndef _TUTTLE_PLUGINCOLORGRADIENTPLUGINFACTORY_HPP_
#define _TUTTLE_PLUGINCOLORGRADIENTPLUGINFACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace colorGradient {
	static const bool   kSupportTiles = true;
	mDeclarePluginFactory( ColorGradientPluginFactory, { }, { } );
}
}
}

#endif