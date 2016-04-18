#ifndef JPEG_WRITER_PLUGIN_FACTORY_HPP
#define JPEG_WRITER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace jpeg
{
namespace writer
{

static const bool kSupportTiles = false;

mDeclarePluginFactory(JpegWriterPluginFactory, {}, {});
}
}
}
}

#endif
