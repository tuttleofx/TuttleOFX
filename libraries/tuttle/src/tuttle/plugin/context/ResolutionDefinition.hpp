#ifndef _TUTTLE_PLUGIN_CONTEXT_RESOLUTION_DEFINITION_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_RESOLUTION_DEFINITION_HPP_

#include <string>

namespace tuttle {
namespace plugin {

static const std::string kParamPCVideo        = "PC-Video            640x480";
static const std::string kParamNTSC           = "NTSC                720x486";
static const std::string kParamPAL            = "PAL                 720x576";
static const std::string kParamHD             = "HD                  1920x1080";
static const std::string kParamNTSC169        = "NTSC-16:9           720x486";
static const std::string kParamPAL169         = "PAL-16:9            720x576";
static const std::string kParam1kSuper35      = "1K-Super35-full-ap  1024x778";
static const std::string kParam1kCinemascope  = "1K-Cinemascope      914x778";
static const std::string kParam2kSuper35      = "2K-Super35-full-ap  2048x1556";
static const std::string kParam2kCinemascope  = "2K-Cinemascope      1828x1556";
static const std::string kParam4kSuper35      = "4K-Super35-full-ap  4096x3112";
static const std::string kParam4kCinemascope  = "4K-Cinemascope      3656x3112";
static const std::string kParamSquare256      = "Square-256          256x256";
static const std::string kParamSquare512      = "Square-512          512x512";
static const std::string kParamSquare1k       = "Square-1k           1024x1024";
static const std::string kParamSquare2k       = "Square-2k           2048x2048";

enum EParamFormat
{
	eParamPCVideo,
	eParamNTSC,
	eParamPAL,
	eParamHD,
	eParamNTSC169,
	eParamPAL169,
	eParam1kSuper35,
	eParam1kCinemascope,
	eParam2kSuper35,
	eParam2kCinemascope,
	eParam4kSuper35,
	eParam4kCinemascope,
	eParamSquare256,
	eParamSquare512,
	eParamSquare1k,
	eParamSquare2k
};

}
}

#endif
