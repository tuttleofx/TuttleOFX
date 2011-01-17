#ifndef _TUTTLE_PLUGIN_CONTEXT_READERDEFINITION_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_READERDEFINITION_HPP_

#include "Definition.hpp"

namespace tuttle {
namespace plugin {

static const std::string kParamReaderFlip           = "flip";
static const std::string kParamReaderFilename           = "filename";
static const std::string kParamReaderExplicitConversion = "explicitConversion";
enum EParamReaderExplicitConversion
{
	eParamReaderExplicitConversionAuto = 0,
	eParamReaderExplicitConversionByte,
	eParamReaderExplicitConversionShort,
	eParamReaderExplicitConversionFloat
};

}
}

#endif

