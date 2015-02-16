#ifndef _TUTTLE_PLUGIN_CONTEXT_GENERATORPLUGIN_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_GENERATORPLUGIN_HPP_

#include "GeneratorDefinition.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <tuttle/plugin/exceptions.hpp>

namespace tuttle {
namespace plugin {

class GeneratorPlugin : public OFX::ImageEffect
{
public:
	GeneratorPlugin( OfxImageEffectHandle handle );
	virtual ~GeneratorPlugin() = 0;

public:
	virtual void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	virtual bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	virtual void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	virtual bool getTimeDomain( OfxRangeD& range );

public:

	EParamGeneratorExplicitConversion getExplicitConversion() const
	{
		return static_cast<EParamGeneratorExplicitConversion>( _paramExplicitConv->getValue() );
	}

	OFX::EBitDepth getOfxExplicitConversion() const
	{
		switch( getExplicitConversion() )
		{
			case eParamGeneratorExplicitConversionByte:
				return OFX::eBitDepthUByte;
			case eParamGeneratorExplicitConversionShort:
				return OFX::eBitDepthUShort;
			case eParamGeneratorExplicitConversionFloat:
				return OFX::eBitDepthFloat;
			case eParamGeneratorExplicitConversionAuto:
				BOOST_THROW_EXCEPTION( exception::Value() );
		}
		return OFX::eBitDepthNone;
	}

protected:
	void updateVisibleTools();

public:
	OFX::Clip*              _clipSrc;  ///< Input image clip
	OFX::Clip*              _clipDst;  ///< Destination image clip
	/// @name user parameters
	/// @{
	OFX::ChoiceParam*       _paramExplicitConv; ///< Explicit conversion
	OFX::ChoiceParam*       _paramComponents;

	OFX::ChoiceParam*       _paramMode;

	OFX::ChoiceParam*       _paramFormat;

	OFX::Int2DParam*        _paramSize;
	OFX::IntParam*          _paramSizeWidth;
	OFX::IntParam*          _paramSizeHeight;
	OFX::BooleanParam*      _paramSizeSpecificRatio;
	OFX::DoubleParam*       _paramSizeRatioValue;
	OFX::ChoiceParam*       _paramSizeOrientation;
	/// @}
};

}
}

#endif

