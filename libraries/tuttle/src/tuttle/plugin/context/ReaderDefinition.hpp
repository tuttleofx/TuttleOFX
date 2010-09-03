#ifndef _TUTTLE_PLUGIN_CONTEXT_READERDEFINITION_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_READERDEFINITION_HPP_

#include "Definition.hpp"

namespace tuttle {
namespace plugin {

static const std::string kReaderParamFilename           = "filename";
static const std::string kReaderParamExplicitConversion = "explicitConversion";
enum EReaderParamExplicitConversion
{
	eReaderParamExplicitConversionAuto = 0,
	eReaderParamExplicitConversionByte,
	eReaderParamExplicitConversionShort,
	eReaderParamExplicitConversionFloat
};

}
}

#endif

