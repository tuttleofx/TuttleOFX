#ifndef OPENIMAGEIO_READER_PLUGIN_FACTORY_HPP
#define OPENIMAGEIO_READER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace openImageIO
{
namespace reader
{

static const bool kSupportTiles = false;

mDeclarePluginFactory(OpenImageIOReaderPluginFactory, {}, {});
}
}
}
}

#endif
