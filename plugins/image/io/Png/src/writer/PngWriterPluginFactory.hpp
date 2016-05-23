#ifndef PNG_WRITER_PLUGIN_FACTORY_HPP
#define PNG_WRITER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace png
{
namespace writer
{

static const bool kSupportTiles = false;

mDeclarePluginFactory(PngWriterPluginFactory, {}, {});
}
}
}
}

#endif
