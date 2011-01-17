#ifndef _DPXWRITER_DEFINITIONS_HPP_
#define _DPXWRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

enum EParamBitDepth
{
	eParamBitDepth8 = 0,
	eParamBitDepth10,
	eParamBitDepth12,
	eParamBitDepth16,
	eParamBitDepth32
};

static const std::string kParamComponentsType = "componentsType";
static const std::string kParamCompressed     = "compressed";

}
}
}
}

#endif
