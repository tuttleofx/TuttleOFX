#ifndef _TUTTLE_PLUGIN_DEBUGIMAGEEFFECTAPI_PLUGIN_FACTORY_HPP_
#define _TUTTLE_PLUGIN_DEBUGIMAGEEFFECTAPI_PLUGIN_FACTORY_HPP_
#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace debugImageEffectApi
{

static const bool kSupportTiles = false;
static const bool kSupportTemporalClipAccess = false;

mDeclarePluginFactory(DebugImageEffectApiPluginFactory, {}, {});
}
}
}

#endif
