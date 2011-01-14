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

}
}
}
}

#endif

