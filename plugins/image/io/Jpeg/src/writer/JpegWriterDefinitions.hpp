#ifndef _JPEGWRITER_DEFINITIONS_HPP_
#define _JPEGWRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/context/WriterDefinition.hpp>
#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace jpeg {
namespace writer {

	enum EParamBitDepth
	{
		eParamBitDepth8 = 0,
		eParamBitDepth16
	};

	static const std::string kParamPremult = "premult";

}
}
}
}

#endif
