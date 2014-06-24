#ifndef _TUTTLE_PLUGIN_AV_COMMON_UTIL_HPP
#define _TUTTLE_PLUGIN_AV_COMMON_UTIL_HPP

#include <AvTranscoder/OptionLoader.hpp>

#include <ofxsImageEffect.h>

#include <string>

namespace tuttle {
namespace plugin {
namespace av {
namespace common {

/**
 * @brief Create OFX parameters depending on the list of Options.
 * @param desc: object to create OFX parameter descriptors
 * @param group: the group to add OFX params
 * @param optionsArray: options to add to the group
 * @param prefix: informaed this to add a prefix to the name of each OFX params create by the function
 */
void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, avtranscoder::OptionLoader::OptionArray& optionsArray, const std::string& prefix="" );

/**
 * @brief: Create OFX parameters depending on the list of Options.
 * @param desc: object to create OFX parameter descriptors
 * @param group: the group to add OFX params
 * @param optionsMap: options to add to the group (organized by the keys in the map)
 */
void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group,  avtranscoder::OptionLoader::OptionMap& optionsMap );

}
}
}
}

#endif
