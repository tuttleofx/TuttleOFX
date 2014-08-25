#ifndef _TUTTLE_PLUGIN_AV_COMMON_UTIL_HPP
#define _TUTTLE_PLUGIN_AV_COMMON_UTIL_HPP

#include <AvTranscoder/OptionLoader.hpp>
#include <AvTranscoder/Profile.hpp>

#include <ofxsImageEffect.h>

#include <string>
#include <utility> //pair
#include <vector>

namespace tuttle {
namespace plugin {
namespace av {
namespace common {

static const size_t prefixSize            = 2;
static const std::string kPrefixFormat    = "f_";
static const std::string kPrefixVideo     = "v_";
static const std::string kPrefixAudio     = "a_";
static const std::string kPrefixMetaData  = "m_";
static const std::string kPrefixAbout     = "l_";

static const std::string kPrefixGroup     = "g_";
static const std::string kPrefixFlag      = "_flag_";

/**
 * @brief Use this struct to get custom Options for format, video, and audio.
 * The Options will be used only if the custom preset is set in the corresponding list of presets.
 */
struct CustomParams
{
public:
	typedef std::pair<std::string, std::string> OptionForPreset;
	typedef std::map< std::string, std::string > OptionsForPreset;
	
	typedef std::vector<std::string> ChildList;
	typedef std::pair< std::string, ChildList > ChildsForChoice;
	typedef std::map< std::string, ChildList > ChildsPerChoice;

public:
	CustomParams()
	: _paramBoolean()
	, _paramInt()
	, _paramDouble()
	, _paramString()
	, _paramRatio()
	, _paramChoice()
	, _childsPerChoice()
	{}

	OptionsForPreset getOptionsNameAndValue( const std::string& subGroupName="" ) const ;

	void fetchCustomParams( OFX::ImageEffect& plugin, avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& prefix="" );
	void fetchCustomParams( OFX::ImageEffect& plugin, avtranscoder::OptionLoader::OptionArray& optionsArray, const std::string& prefix="", const std::string& subGroupName="" );

	/**
	 * @brief Get value of OFX parameters contained in customParams, and return the corresponding profileDesc.
     */
	avtranscoder::Profile::ProfileDesc getCorrespondingProfileDesc( const std::string& subGroupName="" ) const;

public:
	std::vector<OFX::BooleanParam*> _paramBoolean;
	std::vector<OFX::IntParam*> _paramInt;
	std::vector<OFX::DoubleParam*> _paramDouble;
	std::vector<OFX::StringParam*> _paramString;
	std::vector<OFX::Int2DParam*> _paramRatio;
	std::vector<OFX::ChoiceParam*> _paramChoice;
	
	ChildsPerChoice _childsPerChoice;
};

/**
 * @brief Create OFX parameters depending on the list of Options.
 * @param desc: object to create OFX parameter descriptors
 * @param group: the group to add OFX params
 * @param optionsArray: options to add to the group
 * @param prefix: informed this to add a prefix to the name of each OFX params created by the function
 * @param subGroupName: informed this to add an other prefix (the codec name) to the name of each OFX params created by the function
 */
void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, avtranscoder::OptionLoader::OptionArray& optionsArray, const std::string& prefix="", const std::string& subGroupName="" );

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
 * @param optionName with various prefixes (flags_, sub group name...)
 * @param subGroupName a specific prefix (the codec name...)
 * @return the AVOption name
 */
std::string getOptionNameWithoutPrefix( const std::string& optionName, const std::string& subGroupName="" );

/**
 * @brief Get the real flag name of the AVOption (unit).
 * @param optionName with prefix (flags_, sub group name...)
 * @param subGroupName a specific prefix (the codec name...)
 * @return the flag name
 */
std::string getOptionFlagName( const std::string& optionName, const std::string& subGroupName );

/**
 * @brief Disable the OFX parameters named in the optionMap.
 * Use this function to manage the display of OFX custom parameters which are fecth to AVOption of a specific format or codec (video and audio).
 * @param plugin: the plugin which contains the OFX parameters.
 * @param optionsMap: the keys are the name of the subgroup (the format or the codec), and the values are array of Option.
 * @param filter: could be the format or the codec name.
 * @param prefix: prefix of the related OFX parameters.
 */
void disableOFXParamsForFormatOrCodec( OFX::ImageEffect& plugin, avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& filter, const std::string& prefix );

}
}
}
}

#endif
