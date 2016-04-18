#ifndef OPENIMAGEIO_WRITER_PLUGIN_FACTORY_HPP
#define OPENIMAGEIO_WRITER_PLUGIN_FACTORY_HPP
#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace openImageIO
{
namespace writer
{

static const bool kSupportTiles = false;

mDeclarePluginFactory(OpenImageIOWriterPluginFactory, {}, {});
}
}
}
}

#endif
