#ifndef _IMAGEMAGICKWRITER_DEFINITIONS_HPP_
#define _IMAGEMAGICKWRITER_DEFINITIONS_HPP_

#include <tuttle/ioplugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace imagemagick
{
namespace writer
{

enum ETuttlePluginBitDepth
{
    eTuttlePluginBitDepth8 = 0
};

static const std::string kParamPremult = "premult";
static const std::string kParamQuality = "quality";
}
}
}
}

#endif
