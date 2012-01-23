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

static const std::string kParamColorSpaceRgb             = "rgb                 RGB";
static const std::string kParamColorSpaceBt709           = "bt709               BT709     also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / SMPTE RP177 Annex B";
static const std::string kParamColorSpaceUnspecified     = "unspecified         Unspecified";
static const std::string kParamColorSpaceFcc             = "fcc                 FCC";
static const std::string kParamColorSpaceBt470bg         = "bt470bg             BT470BG   also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601";
static const std::string kParamColorSpaceSmpte170m       = "smpte170m           SMPTE170M also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC / functionally identical to above";
static const std::string kParamColorSpaceSmpte240m       = "smpte240m           SMPTE240M";
static const std::string kParamColorSpaceNb              = "nb                  Not part of ABI";

// enum => AVColorSpace in avcodec.h

static const std::string kParamColorPrimaries            = "colorPrimaries";

static const std::string kParamColorPrimariesBt709       = "bt709               BT709     also ITU-R BT1361 / IEC 61966-2-4 / SMPTE RP177 Annex B";
static const std::string kParamColorPrimariesUnspecified = "unspecified         Unspecified";
static const std::string kParamColorPrimariesBt470m      = "bt470m              BT470M";
static const std::string kParamColorPrimariesBt470bg     = "bt470bg             BT470BG   also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM";
static const std::string kParamColorPrimariesSmpte170m   = "smpte170m           SMPTE170M also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC";
static const std::string kParamColorPrimariesSmpte240m   = "smpte240m           SMPTE240M functionally identical to above";
static const std::string kParamColorPrimariesFilm        = "film                Film";
static const std::string kParamColorPrimariesNb          = "nb                  Not part of ABI";

// enum => AVColorPrimaries in avcodec.h

static const std::string kParamColorTRC                  = "colorTransferCharacteristic";

static const std::string kParamColorTRCBt709             = "bt709               BT709     also ITU-R BT1361";
static const std::string kParamColorTRCUnspecified       = "unspecified         Unspecified";
static const std::string kParamColorTRCGamma22           = "gamma22             Gamma 2.2 also ITU-R BT470M / ITU-R BT1700 625 PAL & SECAM";
static const std::string kParamColorTRCGamma28           = "gamma28             Gamma 2.8 also ITU-R BT470BG";
static const std::string kParamColorTRCNb                = "nb                  Not part of ABI";

// enum => AVColorTransferCharacteristic in avcodec.h

static const std::string kParamMotionEstimation          = "motionEstimation";

static const std::string kParamMEZero                    = "zero                no search";
static const std::string kParamMEFull                    = "full                full search";
static const std::string kParamMELog                     = "log                 log motion estimation";
static const std::string kParamMEPhods                   = "phods               parallel hierarchical one-dimensional search";
static const std::string kParamMEEpzs                    = "epzs                enhanced predictive zonal search";
static const std::string kParamMEXl                      = "x1                  reserved for experiments";
static const std::string kParamMEHex                     = "hex                 hexagon based search";
static const std::string kParamMEUmh                     = "umh                 uneven multi-hexagon search";
static const std::string kParamMEIter                    = "iter                iterative search";
static const std::string kParamMETesa                    = "tesa                transformed exhaustive search algorithm";

// enum => Motion_Est_ID in avcodec.h

static const std::string kParamAVDiscard                 = "avDiscard";

static const std::string kParamAVDNothing                = "nothing             discard nothing";
static const std::string kParamAVDPackets                = "packets             discard useless packets like 0 size packets in avi";
static const std::string kParamAVDNonRef                 = "nonRef              discard all non reference";
static const std::string kParamAVDBidir                  = "bidir               discard all bidirectional frames";
static const std::string kParamAVDFrames                 = "frames              discard all frames except keyframes";
static const std::string kParamAVDAll                    = "all                 discard all";

// enum => AVDiscard in avcodec.h

static const std::string kParamAVColorRange              = "avColorRange";

static const std::string kParamAVCRUnspecified           = "unspecified";
static const std::string kParamAVCRMpeg                  = "MPEG                the normal 219*2^(n-8) \"MPEG\" YUV ranges";
static const std::string kParamAVCRJpeg                  = "JPEG                the normal     2^n-1   \"JPEG\" YUV ranges";
static const std::string kParamAVCRNb                    = "nb                  Not part of ABI";

// enum => AVColorRange in avcodec.h

static const std::string kParamAVChromaLocation          = "avChromaLocation";

static const std::string kParamAVCLUnspecified           = "unspecified";
static const std::string kParamAVCLLeft                  = "left               mpeg2/4, h264 default";
static const std::string kParamAVCLCenter                = "center             mpeg1, jpeg, h263";
static const std::string kParamAVCLTopLeft               = "topLeft            DV";
static const std::string kParamAVCLTop                   = "top";
static const std::string kParamAVCLBottomLeft            = "bottomLeft";
static const std::string kParamAVCLBottom                = "bottom";
static const std::string kParamAVCLNb                    = "nb                  Not part of ABI";

// enum => AVChromaLocation in avcodec.h

static const std::string kParamAVAudioServiceType        = "avAudioServiceType";

static const std::string kParamAVASTMain                 = "main";
static const std::string kParamAVASTEffects              = "effects";
static const std::string kParamAVASTVisuallyImpaired     = "visuallyImpaired";
static const std::string kParamAVASTHearingImpaired      = "hearingImpaired";
static const std::string kParamAVASTDialogue             = "dialogue";
static const std::string kParamAVASTCommentary           = "commentary";
static const std::string kParamAVASTEmergency            = "emergency";
static const std::string kParamAVASTVoiceOver            = "voiceOver";
static const std::string kParamAVASTKaraoke              = "karaoke";
static const std::string kParamAVASTNb                   = "nb                  Not part of ABI";

// enum => AVChromaLocation in avcodec.h


}
}
}
}

#endif
