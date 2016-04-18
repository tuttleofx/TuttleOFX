#ifndef PNG_READER_PLUGIN_FACTORY_HPP
#define PNG_READER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace png
{
namespace reader
{

static const bool kSupportTiles = false;

mDeclarePluginFactory(PngReaderPluginFactory, {}, {});
}
}
}
}

#endif
