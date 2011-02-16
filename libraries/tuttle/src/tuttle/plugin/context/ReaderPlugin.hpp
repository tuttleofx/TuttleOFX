#ifndef _TUTTLE_PLUGIN_CONTEXT_READERPLUGIN_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_READERPLUGIN_HPP_

#include "ReaderDefinition.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <tuttle/common/clip/Sequence.hpp>
#include <tuttle/plugin/exceptionsPlugin.hpp>

namespace tuttle {
namespace plugin {

class ReaderPlugin : public OFX::ImageEffect
{
public:
	ReaderPlugin( OfxImageEffectHandle handle );
	virtual ~ReaderPlugin() = 0;

public:
	virtual void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	virtual bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod ) = 0;
	virtual void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	virtual bool getTimeDomain( OfxRangeD& range );

	virtual void render( const OFX::RenderArguments& args );

public:
	std::string getAbsoluteFilenameAt( const OfxTime time ) const
	{
		//		TUTTLE_COUT_VAR( time );
		if( _isSequence )
		{
			//			TUTTLE_COUT_VAR( _filePattern.getAbsoluteFilenameAt( time ) );
			return _filePattern.getAbsoluteFilenameAt( time );
		}
		else
		{
			//			TUTTLE_COUT_VAR( _paramFilepath->getValue() );
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

	EParamReaderExplicitConversion getExplicitConversion() const
	{
		return static_cast<EParamReaderExplicitConversion>( _paramExplicitConv->getValue() );
	}

	OFX::EBitDepth getOfxExplicitConversion() const
	{
		switch( getExplicitConversion() )
		{
			case eParamReaderExplicitConversionByte:
				return OFX::eBitDepthUByte;
			case eParamReaderExplicitConversionShort:
				return OFX::eBitDepthUShort;
			case eParamReaderExplicitConversionFloat:
				return OFX::eBitDepthFloat;
			case eParamReaderExplicitConversionAuto:
				BOOST_THROW_EXCEPTION( exception::Value() );
		}
		return OFX::eBitDepthNone;
	}

protected:
	inline bool varyOnTime() const { return _isSequence; }

public:
	OFX::Clip*           _clipDst;           ///< Destination image clip
	/// @name user parameters
	/// @{
	OFX::StringParam*    _paramFilepath;     ///< File path
	OFX::ChoiceParam*    _paramExplicitConv; ///< Explicit conversion
	OFX::BooleanParam*   _paramFlip;         ///< vertically flip the buffer
	/// @}

private:
	bool _isSequence;
	common::Sequence _filePattern;            ///< Filename pattern manager
};

}
}

#endif

