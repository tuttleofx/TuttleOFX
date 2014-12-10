#ifndef _TUTTLE_PLUGIN_AV_COMMON_UTIL_HPP
#define _TUTTLE_PLUGIN_AV_COMMON_UTIL_HPP

#include <AvTranscoder/util.hpp>
#include <AvTranscoder/option/Context.hpp>
#include <AvTranscoder/ProfileLoader.hpp>

#include <ofxsImageEffect.h>

#include <boost/scoped_ptr.hpp>

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

static const std::string kPrefixEncoding  = "e_";
static const std::string kPrefixDecoding  = "d_";

static const std::string kPrefixGroup     = "g_";
static const std::string kPrefixFlag      = "_flag_";

/**
 * @brief Use this class to get libav Options about format, video, and audio.
 */
class LibAVParams
{
public:
	typedef std::map< std::string, std::string > LibAVOptions;  ///< Key: libav option's name / Value : libav option's value

public:
	/** 
	 * @param prefixScope: format, video, audio, metadata
	 * @param prefixOperation: encoding, decoding
	 */
	LibAVParams( const std::string& prefixScope, const std::string& prefixOperation );

	/**
	 * @brief Get all FFmpeg options and values corresponding to the OFX parameters contains in the object.
	 */
	LibAVOptions getLibAVOptions( const std::string& subGroupName="" ) const ;

	void fetchLibAVParams( OFX::ImageEffect& plugin, avtranscoder::OptionArrayMap& optionArrayMap, const std::string& prefix="" );
	void fetchLibAVParams( OFX::ImageEffect& plugin, avtranscoder::OptionArray& optionsArray, const std::string& prefix="", const std::string& subGroupName="" );

	/**
	 * @brief Get value of OFX parameters contained in the object, and return the corresponding profileDesc.
	 * @note the result does not contain options which have a current value equals to the default value
	 */
	avtranscoder::ProfileLoader::Profile getCorrespondingProfile( const std::string& subGroupName="" ) const;
	
	/**
	 * @brief Set the FFmpeg option and use its value to set the corresponding OFX parameter
	 * @param libAVOptionName: the option whithout all prefixes.
	 * @param value: the value will be cast to the corresponding type (int, double...).
	 * @param subGroupName
	 * @param prefix
	 */
	void setOption( const std::string& libAVOptionName, const std::string& value, const std::string& prefix, const std::string& subGroupName="" );

	/**
	 * @brief Get the OFX parameter which corresponds to the FFmpeg option name (whithout any prefixes).
	 * @note return NULL if not found.
	 */
	OFX::ValueParam* getOFXParameter( const std::string& libAVOptionName, const std::string& subGroupName="" ) ;

public:
	/**
	 * @brief Contains several OFX parameters with different type.
	 * Real type could be:
	 * - BooleanParam
	 * - IntParam
	 * - DoubleParam
	 * - StringParam
	 * - Int2DParam
	 * - ChoiceParam
	 */
	std::vector<OFX::ValueParam*> _paramOFX;

	std::map< OFX::ChoiceParam*, std::vector<std::string> > _childsPerChoice;  ///< List of values per OFX Choice

	/**
	 * @brief Hold the libav options.
	 */
	boost::scoped_ptr<avtranscoder::Context> _avContext;
};

/**
 * @brief Create OFX parameters depending on the list of Options.
 * @param desc: object to create OFX parameter descriptors
 * @param group: the group to add OFX params
 * @param optionsArray: options to add to the group
 * @param prefix: informed this to add a prefix to the name of each OFX params created by the function
 * @param subGroupName: informed this to add an other prefix (the codec name) to the name of each OFX params created by the function
 */
void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, avtranscoder::OptionArray& optionsArray, const std::string& prefix="", const std::string& subGroupName="" );

/**
 * @brief: Create OFX parameters depending on the list of Options.
 * @param desc: object to create OFX parameter descriptors
 * @param group: the group to add OFX params
 * @param optionArrayMap: options to add to the group (organized by the keys in the map)
 * @param prefix: informed this to add a prefix to the name of each OFX params created by the function
 */
void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group,  avtranscoder::OptionArrayMap& optionArrayMap, const std::string& prefix="" );

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
 * @param optionArrayMap: the keys are the name of the subgroup (the format or the codec), and the values are array of Option.
 * @param filter: could be the format or the codec name.
 * @param prefix: prefix of the related OFX parameters.
 */
void disableOFXParamsForFormatOrCodec( OFX::ImageEffect& plugin, avtranscoder::OptionArrayMap& optionArrayMap, const std::string& filter, const std::string& prefix );

}
}
}
}

#endif
