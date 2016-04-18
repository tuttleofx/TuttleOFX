#ifndef EXR_WRITER_PLUGIN_FACTORY_HPP
#define EXR_WRITER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace exr
{
namespace writer
{

static const bool kSupportTiles = false;
mDeclarePluginFactory(EXRWriterPluginFactory, {}, {});
}
}
}
}

#endif // EXR_WRITER_PLUGIN_FACTORY_HPP
