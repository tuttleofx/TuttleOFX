#ifndef IMAGEMAGICK_READER_PLUGIN_FACTORY_HPP
#define IMAGEMAGICK_READER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace imagemagick
{
namespace reader
{

static const bool kSupportTiles = false;

mDeclarePluginFactory(ImageMagickReaderPluginFactory, {}, {});
}
}
}
}

#endif
