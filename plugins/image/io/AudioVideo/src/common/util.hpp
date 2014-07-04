#ifndef _TUTTLE_PLUGIN_AV_COMMON_UTIL_HPP
#define _TUTTLE_PLUGIN_AV_COMMON_UTIL_HPP

#include <AvTranscoder/OptionLoader.hpp>

#include <ofxsImageEffect.h>

#include <string>
#include <utility> //pair
#include <vector>

namespace tuttle {
namespace plugin {
namespace av {
namespace common {

/**
 * @brief Use this struct to get custom Options for format, video, and audio.
 * The Options will be used only if the custom preset is set in the corresponding list of presets.
 */
struct CustomParams
{
public:
	typedef std::pair<std::string, std::string> OptionForPreset;
	typedef std::vector< OptionForPreset > OptionsForPreset;

public:
	CustomParams()
	: _paramBoolean()
	, _paramInt()
	, _paramDouble()
	, _paramString()
	, _paramRatio()
	, _paramChoice()
	{}

	OptionsForPreset getOptionsNameAndValue( const std::string& codecName );

public:
	std::vector<OFX::BooleanParam*> _paramBoolean;
	std::vector<OFX::IntParam*> _paramInt;
	std::vector<OFX::DoubleParam*> _paramDouble;
	std::vector<OFX::StringParam*> _paramString;
	std::vector<OFX::Int2DParam*> _paramRatio;
	std::vector<OFX::ChoiceParam*> _paramChoice;
};

/**
 * @brief Create OFX parameters depending on the list of Options.
 * @param desc: object to create OFX parameter descriptors
 * @param group: the group to add OFX params
 * @param optionsArray: options to add to the group
 * @param prefix: informed this to add a prefix to the name of each OFX params created by the function
 */
void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, avtranscoder::OptionLoader::OptionArray& optionsArray, const std::string& prefix="" );

/**
 * @brief: Create OFX parameters depending on the list of Options.
 * @param desc: object to create OFX parameter descriptors
 * @param group: the group to add OFX params
 * @param optionsMap: options to add to the group (organized by the keys in the map)
 * @param prefix: informed this to add a prefix to the name of each OFX params created by the function
 */
void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group,  avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& prefix="" );

/**
 * @brief Get the real name of the AVOption, without our prefix.
 * @param optionName with prefix (g_, flag_...)
 * @return the AVOption name
 */
std::string getOptionNameWithoutPrefix( const std::string& optionName, const std::string& codecName="" );

}
}
}
}

#endif
