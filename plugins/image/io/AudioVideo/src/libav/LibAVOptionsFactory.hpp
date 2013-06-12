#ifndef _TUTTLE_PLUGIN_LIBAV_LIBAVOPTIONSFACTORY_HPP_
#define _TUTTLE_PLUGIN_LIBAV_LIBAVOPTIONSFACTORY_HPP_

#include "LibAV.hpp"

#include <ofxsImageEffect.h>

#include <vector>

namespace tuttle {
namespace plugin {
namespace av {

void addOptionsFromAVOption( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, void* av_class, int req_flags, int rej_flags );

void addOptionsFromAVOption( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, const std::vector<AVPrivOption>& options );

}
}
}
#endif
