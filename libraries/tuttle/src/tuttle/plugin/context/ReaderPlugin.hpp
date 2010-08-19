#ifndef _TUTTLE_PLUGIN_CONTEXT_READERPLUGIN_HPP_
#define	_TUTTLE_PLUGIN_CONTEXT_READERPLUGIN_HPP_

#include "ReaderDefinition.hpp"

#include <tuttle/common/clip/Sequence.hpp>

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

public:
	std::string getAbsoluteFilenameAt( const OfxTime time ) const
	{
//		COUT_VAR( time );
		if( _isSequence )
		{
//			COUT_VAR( _filePattern.getAbsoluteFilenameAt( time ) );
			return _filePattern.getAbsoluteFilenameAt( time );
		}
		else
		{
//			COUT_VAR( _paramFilepath->getValue() );
			return _paramFilepath->getValue();
		}
	}
	std::string getAbsoluteFirstFilename() const
	{
		if( _isSequence )
			return _filePattern.getAbsoluteFirstFilename();
		else
			return _paramFilepath->getValue();
	}
	OfxTime getFirstTime() const
	{
		if( _isSequence )
			return _filePattern.getFirstTime();
		else
			return kOfxFlagInfiniteMin;
	}
	OfxTime getLastTime() const
	{
		if( _isSequence )
			return _filePattern.getLastTime();
		else
			return kOfxFlagInfiniteMax;
	}

protected:
	inline bool varyOnTime() const { return _isSequence; }

public:
	OFX::Clip*           _clipDst;           ///< Destination image clip
	/// @name user parameters
	/// @{
	OFX::StringParam*    _paramFilepath;     ///< File path
	OFX::ChoiceParam*    _paramExplicitConv; ///< Explicit conversion
	/// @}
	
private:
	bool                  _isSequence;
	common::Sequence      _filePattern;       ///< Filename pattern manager
};


}
}

#endif

