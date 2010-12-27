#ifndef EXRWRITER_DEFINITIONS_HPP
#define EXRWRITER_DEFINITIONS_HPP

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

enum ECompType { eGray, eRGB, eRGBA };

enum EParamBitDepth
{
	eParamBitDepth16f = 0,
	eParamBitDepth32f,
	eParamBitDepth32
};

static const std::string kParamComponentsType = "compType";

const static std::string kEXRWriterHelpButton = "Help";
const static std::string kEXRWriterHelpString = "<b>EXR File writer</b> plugin is used to write exr files.<br />";

}
}
}
}

#endif

