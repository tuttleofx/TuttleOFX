#ifndef IMAGEMAGICK_WRITER_PLUGIN_FACTORY_HPP
#define IMAGEMAGICK_WRITER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace imagemagick
{
namespace writer
{

static const bool kSupportTiles = false;

mDeclarePluginFactory(ImageMagickWriterPluginFactory, {}, {});
}
}
}
}

#endif
