#ifndef _TUTTLE_PLUGIN_CONTEXT_READERPLUGIN_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_READERPLUGIN_HPP_

#include <boost/gil/channel_algorithm.hpp> // force to use the boostHack version first

#include "ReaderDefinition.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <Sequence.hpp>
#include <tuttle/plugin/exceptions.hpp>


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
		//TUTTLE_LOG_VAR( TUTTLE_INFO, time );
		if( _isSequence )
		{
			//TUTTLE_LOG_VAR( TUTTLE_INFO, _filePattern.getAbsoluteFilenameAt( time ) );
			return _filePattern.getAbsoluteFilenameAt( static_cast<std::ssize_t>(time) );
		}
		else
		{
			//TUTTLE_LOG_VAR( TUTTLE_INFO, _paramFilepath->getValue() );
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

	EParamReaderBitDepth getExplicitBitDepthConversion() const
	{
		return static_cast<EParamReaderBitDepth>( _paramBitDepth->getValue() );
	}

	
	EParamReaderChannel getExplicitChannelConversion() const
	{
		return static_cast<EParamReaderChannel>( _paramChannel->getValue() );
	}
	
	OFX::EBitDepth getOfxExplicitConversion() const
	{
		switch( getExplicitBitDepthConversion() )
		{
			case eParamReaderBitDepthByte:
				return OFX::eBitDepthUByte;
			case eParamReaderBitDepthShort:
				return OFX::eBitDepthUShort;
			case eParamReaderBitDepthFloat:
				return OFX::eBitDepthFloat;
			case eParamReaderBitDepthAuto:
				BOOST_THROW_EXCEPTION( exception::Value() );
		}
		return OFX::eBitDepthNone;
	}

protected:
	virtual inline bool varyOnTime() const { return _isSequence; }

public:
	OFX::Clip*           _clipDst;        ///< Destination image clip
	/// @name user parameters
	/// @{
	OFX::StringParam*    _paramFilepath;  ///< File path
	OFX::ChoiceParam*    _paramBitDepth;  ///< Explicit bit depth conversion
	OFX::ChoiceParam*    _paramChannel;   ///< Explicit component conversion
	/// @}

private:
	bool _isSequence;
	sequenceParser::Sequence _filePattern;            ///< Filename pattern manager
};

}
}

#endif

