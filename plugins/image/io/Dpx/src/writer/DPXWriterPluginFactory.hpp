#ifndef _DPX_WRITER_PLUGIN_FACTORY_HPP_
#define _DPX_WRITER_PLUGIN_FACTORY_HPP_
#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace dpx
{
namespace writer
{

static const bool kSupportTiles = false;

mDeclarePluginFactory(DPXWriterPluginFactory, {}, {});
}
}
}
}

#endif
