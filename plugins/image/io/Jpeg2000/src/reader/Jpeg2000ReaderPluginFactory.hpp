#ifndef _TUTTLE_PLUGIN_JPEG2000_READER_PLUGIN_FACTORY_HPP_
#define _TUTTLE_PLUGIN_JPEG2000_READER_PLUGIN_FACTORY_HPP_
#include <ofxsImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace jpeg2000
{
namespace reader
{

static const bool kSupportTiles = false;

mDeclarePluginFactory(Jpeg2000ReaderPluginFactory, {}, {});
}
}
}
}

#endif
