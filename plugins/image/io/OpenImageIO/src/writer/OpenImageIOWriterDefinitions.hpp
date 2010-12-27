#ifndef _OPENIMAGEIOWRITER_DEFINITIONS_HPP_
#define _OPENIMAGEIOWRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace writer {

enum EParamBitDepth
{
	eParamBitDepth8 = 0,
	eParamBitDepth16,
	eParamBitDepth32
};

static const std::string kParamOutputComponents     = "components";
static const std::string kParamOutputComponentsRGBA = "rgba";
static const std::string kParamOutputComponentsRGB  = "rgb";

enum EParamComponents
{
	eParamComponentsRGBA = 0,
	eParamComponentsRGB
};
}
}
}
}

#endif
