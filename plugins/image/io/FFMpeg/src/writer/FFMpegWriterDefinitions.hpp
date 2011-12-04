#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

static const std::string kParamFormat           = "format";
static const std::string kParamCodec            = "codec";
static const std::string kParamBitrate          = "bitrate";
static const std::string kParamMotionEstimation = "motionEstimation";

static const std::string kParamMEZero           = "zero   - no search";
static const std::string kParamMEFull           = "full   - full search";
static const std::string kParamMELog            = "log    - log motion estimation";
static const std::string kParamMEPhods          = "phods  - parallel hierarchical one-dimensional search";
static const std::string kParamMEEpzs           = "epzs   - enhanced predictive zonal search";
static const std::string kParamMEXl             = "x1     - reserved for experiments";
static const std::string kParamMEHex            = "hex    - hexagon based search";
static const std::string kParamMEUmh            = "umh    - uneven multi-hexagon search";
static const std::string kParamMEIter           = "iter   - iterative search";
static const std::string kParamMETesa           = "tesa   - transformed exhaustive search algorithm";

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
