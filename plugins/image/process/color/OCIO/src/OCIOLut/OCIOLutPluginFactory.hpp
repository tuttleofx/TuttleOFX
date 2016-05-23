#ifndef _TUTTLE_PLUGIN_OCIOLUTPLUGINFACTORY_HPP_
#define _TUTTLE_PLUGIN_OCIOLUTPLUGINFACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace ocio
{
namespace lut
{

static const bool kSupportTiles = true;
mDeclarePluginFactory(OCIOLutPluginFactory, {}, {});
}
}
}
}

#endif
