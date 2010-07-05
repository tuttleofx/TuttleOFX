#ifndef _TUTTLE_PLUGIN_JPEG2000_WRITER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_JPEG2000_WRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/context/WriterDefinition.hpp>
#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace writer {

	enum EParamBitDepth
	{
		eParamBitDepth8 = 0,
		eParamBitDepth12,
		eParamBitDepth16,
		eParamBitDepth32
	};


static const std::string kParamCinemaProfil       = "cinemaProfil";
static const std::string kParamLossless           = "lossless";

const static std::string kJpeg2000HelpButton = "Help";
const static std::string kJ2KHelpString = "<b>Jpeg2000 io</b> plugin is used to output jpeg 2000 files.<br />In the filename pattern, put @ where you want your incrementation to be.<br />";

}
}
}
}

#endif
