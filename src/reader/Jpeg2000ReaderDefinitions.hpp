#ifndef _TUTTLE_PLUGIN_JPEG2000_READER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_JPEG2000_READER_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace reader {

enum EParamBitDepth
{
	eParamBitDepthAuto = 0,
	eParamBitDepth8,
	eParamBitDepth16,
	eParamBitDepth32
};

const static std::string kJpeg2000HelpButton = "Help";
const static std::string kJpeg2000HelpString = "<b>Jpeg200 io</b> plugin is used to read jpeg 2000 files.  <br />";

}
}
}
}

#endif
