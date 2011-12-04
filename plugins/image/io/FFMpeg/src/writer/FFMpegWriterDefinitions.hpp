#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

static const std::string kParamFormat                    = "format";
static const std::string kParamCodec                     = "codec";
static const std::string kParamBitrate                   = "bitrate";

static const std::string kParamColorSpace                = "colorSpace";
static const std::string kParamColorSpaceBt709           = "bt709               BT709     also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / SMPTE RP177 Annex B";
static const std::string kParamColorSpaceUnspecified     = "unspecified         Unspecified";
static const std::string kParamColorSpaceFcc             = "fcc                 FCC";
static const std::string kParamColorSpaceBt470bg         = "bt470bg             BT470BG   also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601";
static const std::string kParamColorSpaceSmpte170m       = "smpte170m           SMPTE170M also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC / functionally identical to above";
static const std::string kParamColorSpaceSmpte240m       = "smpte240m           SMPTE240M";
static const std::string kParamColorSpaceNb              = "nb                  Not part of ABI";

enum EParamColorSpace
{
    eParamColorSpaceBt709 = 0,
    eParamColorSpaceUnspecified,
    eParamColorSpaceFcc,
    eParamColorSpaceBt470bg,
    eParamColorSpaceSmpte170m,
    eParamColorSpaceSmpte240m,
    eParamColorSpaceNb
};

static const std::string kParamColorPrimaries            = "colorPrimaries";

static const std::string kParamColorPrimariesBt709       = "bt709               BT709     also ITU-R BT1361 / IEC 61966-2-4 / SMPTE RP177 Annex B";
static const std::string kParamColorPrimariesUnspecified = "unspecified         Unspecified";
static const std::string kParamColorPrimariesBt470m      = "bt470m              BT470M";
static const std::string kParamColorPrimariesBt470bg     = "bt470bg             BT470BG   also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM";
static const std::string kParamColorPrimariesSmpte170m   = "smpte170m           SMPTE170M also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC";
static const std::string kParamColorPrimariesSmpte240m   = "smpte240m           SMPTE240M functionally identical to above";
static const std::string kParamColorPrimariesFilm        = "film                Film";
static const std::string kParamColorPrimariesNb          = "nb                  Not part of ABI";

enum EParamColorPrimaries
{
    eParamColorPrimariesBt709 = 0,
    eParamColorPrimariesUnspecified,
    eParamColorPrimariesBt470m,
    eParamColorPrimariesBt470bg,
    eParamColorPrimariesSmpte170m,
    eParamColorPrimariesSmpte240m,
    eParamColorPrimariesFilm,
    eParamColorPrimariesNb
};

static const std::string kParamMotionEstimation = "motionEstimation";

static const std::string kParamMEZero           = "zero                no search";
static const std::string kParamMEFull           = "full                full search";
static const std::string kParamMELog            = "log                 log motion estimation";
static const std::string kParamMEPhods          = "phods               parallel hierarchical one-dimensional search";
static const std::string kParamMEEpzs           = "epzs                enhanced predictive zonal search";
static const std::string kParamMEXl             = "x1                  reserved for experiments";
static const std::string kParamMEHex            = "hex                 hexagon based search";
static const std::string kParamMEUmh            = "umh                 uneven multi-hexagon search";
static const std::string kParamMEIter           = "iter                iterative search";
static const std::string kParamMETesa           = "tesa                transformed exhaustive search algorithm";

enum EParamMotionEstimation
{
    eParamMEZero = 0,
    eParamMEFull,
    eParamMELog,
    eParamMEPhods,
    eParamMEEpzs,
    eParamMEXl,
    eParamMEHex,
    eParamMEUmh,
    eParamMEIter,
    eParamMETesa
};

}
}
}
}

#endif
