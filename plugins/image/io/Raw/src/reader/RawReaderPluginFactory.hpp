#ifndef _TUTTLE_PLUGIN_RAWREADERPLUGINFACTORY_HPP_
#define _TUTTLE_PLUGIN_RAWREADERPLUGINFACTORY_HPP_
#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace raw
{
namespace reader
{

static const bool kSupportTiles = false;

mDeclarePluginFactory(RawReaderPluginFactory, {}, {});
}
}
}
}

#endif
