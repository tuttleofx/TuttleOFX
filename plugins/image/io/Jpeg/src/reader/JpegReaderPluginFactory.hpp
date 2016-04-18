#ifndef JPEG_READER_PLUGIN_FACTORY_HPP
#define JPEG_READER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace jpeg
{
namespace reader
{

static const bool kSupportTiles = false;

mDeclarePluginFactory(JpegReaderPluginFactory, {}, {});
}
}
}
}

#endif
