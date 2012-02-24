#ifndef _TUTTLE_PLUGIN_JPEG2000_WRITER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_JPEG2000_WRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>


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


static const std::string kParamCinemaProfil        = "cinemaProfil";

static const std::string kParamCinemaProfilNoDigit = "notDigt Not Digital Cinema";
static const std::string kParamCinemaProfil2k24fps = "2k24fps 2K Digital Cinema at 24 fps";
static const std::string kParamCinemaProfil2k48fps = "2k48fps 2K Digital Cinema at 48 fps";
static const std::string kParamCinemaProfil4k24fps = "4k24fps 4K Digital Cinema at 24 fps";

static const std::string kParamLossless            = "lossless";

}
}
}
}

#endif
