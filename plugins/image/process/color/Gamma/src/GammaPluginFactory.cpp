#include "GammaPluginFactory.hpp"
#include "GammaPlugin.hpp"
#include "GammaDefinitions.hpp"

#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace gamma {

static const bool kSupportTiles = true;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void GammaPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleGamma", "Gamma",
	                "Apply gamma value on image." );
	desc.setPluginGrouping( "tuttle/image/process/color" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void GammaPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                            OFX::EContext               context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );

	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::ChoiceParamDescriptor* gammaType = desc.defineChoiceParam( kGammaType );
	gammaType->setLabel( "Type" );
	gammaType->setHint( "Gamma selection mode." );
	gammaType->appendOption( kGammaGlobal );
	gammaType->appendOption( kGammaChannels );

	OFX::DoubleParamDescriptor* masterValue = desc.defineDoubleParam( kMasterValue );
	masterValue->setLabel( "Master" );
	masterValue->setRange( 0.001, 1024.0 );
	masterValue->setDisplayRange( 0.001, 20.0 );
	masterValue->setDoubleType( OFX::eDoubleTypePlain );
	masterValue->setDefault( 1.0 );

	OFX::DoubleParamDescriptor* redValue = desc.defineDoubleParam( kRedValue );
	redValue->setLabel( "Red" );
	redValue->setRange( 0.001, 1024.0 );
	redValue->setDisplayRange( 0.001, 20.0 );
	redValue->setDoubleType( OFX::eDoubleTypePlain );
	redValue->setDefault( 1.0 );

	OFX::DoubleParamDescriptor* greenValue = desc.defineDoubleParam( kGreenValue );
	greenValue->setLabel( "Green" );
	greenValue->setRange( 0.001, 1024.0 );
	greenValue->setDisplayRange( 0.001, 20.0 );
	greenValue->setDoubleType( OFX::eDoubleTypePlain );
	greenValue->setDefault( 1.0 );

	OFX::DoubleParamDescriptor* blueValue = desc.defineDoubleParam( kBlueValue );
	blueValue->setLabel( "Blue" );
	blueValue->setRange( 0.001, 1024.0 );
	blueValue->setDisplayRange( 0.001, 20.0 );
	blueValue->setDoubleType( OFX::eDoubleTypePlain );
	blueValue->setDefault( 1.0 );

	OFX::DoubleParamDescriptor* alphaValue = desc.defineDoubleParam( kAlphaValue );
	alphaValue->setLabel( "Alpha" );
	alphaValue->setRange( 0.001, 1024.0 );
	alphaValue->setDisplayRange( 0.001, 20.0 );
	alphaValue->setDoubleType( OFX::eDoubleTypePlain );
	alphaValue->setDefault( 1.0 );

	OFX::BooleanParamDescriptor* invert = desc.defineBooleanParam( kInvert );
	invert->setDefault( false );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* GammaPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                      OFX::EContext        context )
{
	return new GammaPlugin( handle );
}

}
}
}

