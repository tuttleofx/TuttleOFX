#ifndef _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_FACTORY_HPP_
#define _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_FACTORY_HPP_

#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace av
{
namespace writer
{

static const bool kSupportTiles = false;

mDeclarePluginFactory(AVWriterPluginFactory, {}, {});
}
}
}
}

#endif
