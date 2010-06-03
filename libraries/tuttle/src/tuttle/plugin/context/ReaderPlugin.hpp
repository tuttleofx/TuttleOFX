#ifndef _TUTTLE_PLUGIN_CONTEXT_READERPLUGIN_HPP_
#define	_TUTTLE_PLUGIN_CONTEXT_READERPLUGIN_HPP_

#include "ReaderDefinition.hpp"
#include <tuttle/plugin/FilenameManager.hpp>

#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {

class ReaderPlugin : public OFX::ImageEffect
{
public:
	ReaderPlugin( OfxImageEffectHandle handle );
	virtual ~ReaderPlugin();

public:
	virtual void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	virtual bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod ) = 0;
	virtual void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences ) = 0;
	virtual bool getTimeDomain( OfxRangeD& range );

protected:
	inline bool varyOnTime() const;

public:
	OFX::Clip*           _clipDst;           ///< Destination image clip
	/// @name user parameters
	/// @{
	OFX::StringParam*    _paramFilepath;     ///< File path
	OFX::ChoiceParam*    _paramExplicitConv; ///< Explicit conversion
	/// @}
	FilenameManager      _filePattern;       ///< Filename pattern manager
};

inline bool ReaderPlugin::varyOnTime() const
{
	///@todo tuttle: do this in FilenameManager
	return _filePattern.getFirstFilename() != _paramFilepath->getValue();
}

}
}

#endif

