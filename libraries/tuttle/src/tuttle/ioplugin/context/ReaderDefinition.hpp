#ifndef _TUTTLE_IOPLUGIN_CONTEXT_READERDEFINITION_HPP_
#define _TUTTLE_IOPLUGIN_CONTEXT_READERDEFINITION_HPP_

#include <tuttle/plugin/context/Definition.hpp>

namespace tuttle {
namespace plugin {

enum EParamReaderBitDepth
{
	eParamReaderBitDepthAuto = 0,
	eParamReaderBitDepthByte,
	eParamReaderBitDepthShort,
	eParamReaderBitDepthFloat
};

enum EParamReaderChannel
{
	eParamReaderChannelAuto = 0,
	eParamReaderChannelGray,
	eParamReaderChannelRGB,
	eParamReaderChannelRGBA
};

}
}

#endif

