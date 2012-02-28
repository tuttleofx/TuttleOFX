#ifndef _JPEGWRITER_DEFINITIONS_HPP_
#define _JPEGWRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace writer {

enum ETuttlePluginBitDepth
{
	eTuttlePluginBitDepth8
};

static const std::string kParamPremult = "premult";
static const std::string kParamQuality = "quality";

}
}
}
}

#endif
