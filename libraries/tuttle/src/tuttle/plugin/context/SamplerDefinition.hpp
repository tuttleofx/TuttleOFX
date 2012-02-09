#ifndef _TUTTLE_PLUGIN_CONTEXT_SAMPLER_DEFINITION_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_SAMPLER_DEFINITION_HPP_

#include <terry/sampler/sampler.hpp>

#include <string>



namespace tuttle {
namespace plugin {

static const std::string kParamFilter                = "filter";
static const std::string kParamFilterLabel           = "Filter";

static const std::string kParamFilterNearest         = "nearest";
static const std::string kParamFilterBilinear        = "bilinear";
static const std::string kParamFilterBC              = "bc";
static const std::string kParamFilterBicubic         = "bicubic";
static const std::string kParamFilterCatrom          = "catmul-rom";
static const std::string kParamFilterKeys            = "keys";
static const std::string kParamFilterSimon           = "simon";
static const std::string kParamFilterRifman          = "rifman";
static const std::string kParamFilterMitchell        = "mitchell";
static const std::string kParamFilterParzen          = "parzen";
static const std::string kParamFilterLanczos         = "lanczos";
static const std::string kParamFilterLanczos3        = "lanczos3";
static const std::string kParamFilterLanczos4        = "lanczos4";
static const std::string kParamFilterLanczos6        = "lanczos6";
static const std::string kParamFilterLanczos12       = "lanczos12";
static const std::string kParamFilterGaussian        = "gaussian";

static const std::string kParamFilterB               = "b";
static const std::string kParamFilterC               = "c";

static const std::string kParamFilterSize            = "filterSize";
static const std::string kParamFilterSharpen         = "filterSharpen";
static const std::string kParamFilterSigma           = "filterSigma";

static const std::string kParamFilterOutOfImage      = "outOfImage";
static const std::string kParamFilterOutBlack        = "black";
static const std::string kParamFilterOutTransparency = "transparency";
static const std::string kParamFilterOutCopy         = "copy";
static const std::string kParamFilterOutMirror       = "mirror";


}
}
#endif
