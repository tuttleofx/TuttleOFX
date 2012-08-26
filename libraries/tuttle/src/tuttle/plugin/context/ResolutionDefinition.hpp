#ifndef _TUTTLE_PLUGIN_CONTEXT_RESOLUTION_DEFINITION_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_RESOLUTION_DEFINITION_HPP_

#include <string>

namespace tuttle {
namespace plugin {

static const std::string kParamFormatPCVideo        = "PC-Video";
static const std::string kParamFormatPCVideoLabel   = "PC-Video            640x480";
static const std::string kParamFormatNTSC           = "NTSC";
static const std::string kParamFormatNTSCLabel      = "NTSC                720x486";
static const std::string kParamFormatPAL            = "PAL";
static const std::string kParamFormatPALLabel       = "PAL                 720x576";
static const std::string kParamFormatHD             = "HD";
static const std::string kParamFormatHDLabel        = "HD                  1920x1080";
static const std::string kParamFormatNTSC169        = "NTSC-16:9";
static const std::string kParamFormatNTSC169Label   = "NTSC-16:9           720x486";
static const std::string kParamFormatPAL169         = "PAL-16:9";
static const std::string kParamFormatPAL169Label    = "PAL-16:9            720x576";
static const std::string kParamFormat1kSuper35      = "1K-Super35-full-ap";
static const std::string kParamFormat1kSuper35Label = "1K-Super35-full-ap  1024x778";
static const std::string kParamFormat1kCinemascope  = "1K-Cinemascope";
static const std::string kParamFormat1kCinemascopeLabel = "1K-Cinemascope      914x778";
static const std::string kParamFormat2kSuper35      = "2K-Super35-full-ap";
static const std::string kParamFormat2kSuper35Label = "2K-Super35-full-ap  2048x1556";
static const std::string kParamFormat2kCinemascope  = "2K-Cinemascope";
static const std::string kParamFormat2kCinemascopeLabel = "2K-Cinemascope      1828x1556";
static const std::string kParamFormat4kSuper35      = "4K-Super35-full-ap";
static const std::string kParamFormat4kSuper35Label = "4K-Super35-full-ap  4096x3112";
static const std::string kParamFormat4kCinemascope  = "4K-Cinemascope";
static const std::string kParamFormat4kCinemascopeLabel = "4K-Cinemascope      3656x3112";
static const std::string kParamFormatSquare256      = "Square-256";
static const std::string kParamFormatSquare256Label = "Square-256          256x256";
static const std::string kParamFormatSquare512      = "Square-512";
static const std::string kParamFormatSquare512Label = "Square-512          512x512";
static const std::string kParamFormatSquare1k       = "Square-1k";
static const std::string kParamFormatSquare1kLabel  = "Square-1k           1024x1024";
static const std::string kParamFormatSquare2k       = "Square-2k";
static const std::string kParamFormatSquare2kLabel  = "Square-2k           2048x2048";

enum EParamFormat
{
	eParamFormatPCVideo,
	eParamFormatNTSC,
	eParamFormatPAL,
	eParamFormatHD,
	eParamFormatNTSC169,
	eParamFormatPAL169,
	eParamFormat1kSuper35,
	eParamFormat1kCinemascope,
	eParamFormat2kSuper35,
	eParamFormat2kCinemascope,
	eParamFormat4kSuper35,
	eParamFormat4kCinemascope,
	eParamFormatSquare256,
	eParamFormatSquare512,
	eParamFormatSquare1k,
	eParamFormatSquare2k
};

inline void getFormatResolution( const EParamFormat format, std::size_t& width, std::size_t& height )
{
	switch( format )
	{
		case eParamFormatPCVideo:       width =  640; height =  480; break;
		case eParamFormatNTSC:          width =  720; height =  486; break;
		case eParamFormatPAL:           width =  720; height =  576; break;
		case eParamFormatHD:            width = 1920; height = 1080; break;
		case eParamFormatNTSC169:       width =  720; height =  486; break;
		case eParamFormatPAL169:        width =  720; height =  576; break;
		case eParamFormat1kSuper35:     width = 1024; height =  778; break;
		case eParamFormat1kCinemascope: width =  914; height =  778; break;
		case eParamFormat2kSuper35:     width = 2048; height = 1556; break;
		case eParamFormat2kCinemascope: width = 1828; height = 1556; break;
		case eParamFormat4kSuper35:     width = 4096; height = 3112; break;
		case eParamFormat4kCinemascope: width = 3656; height = 3112; break;
		case eParamFormatSquare256:     width =  256; height =  256; break;
		case eParamFormatSquare512:     width =  512; height =  512; break;
		case eParamFormatSquare1k:      width = 1024; height = 1024; break;
		case eParamFormatSquare2k:      width = 2048; height = 2048; break;
	}
}

}
}

#endif
