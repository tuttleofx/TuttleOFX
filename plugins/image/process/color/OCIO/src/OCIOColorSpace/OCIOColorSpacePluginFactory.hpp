#ifndef _TUTTLE_PLUGIN_OCIOCOLORSPACEPLUGINFACTORY_HPP_
#define _TUTTLE_PLUGIN_OCIOCOLORSPACEPLUGINFACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace ocio
{
namespace colorspace
{

static const bool kSupportTiles = true;
mDeclarePluginFactory(OCIOColorSpacePluginFactory, {}, {});
}
}
}
}

#endif
