#include "ColorMatrixPluginFactory.hpp"
#include "ColorMatrixPlugin.hpp"
#include "ColorMatrixDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle {
namespace plugin {
namespace colorMatrix {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ColorMatrixPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels(
		"ColorMatrix",
		"ColorMatrix",
		"ColorMatrix" );
	desc.setPluginGrouping( "tuttle" );

	desc.setDescription( "Plugin under early development." );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
	desc.setHostFrameThreading( true );
	desc.setSupportsMultiResolution( true );
	desc.setSupportsMultipleClipDepths( true );
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ColorMatrixPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::RGBAParamDescriptor* outputRed = desc.defineRGBAParam( kRedMixedValues );
	outputRed->setDefault( 1, 0, 0, 0 );
	outputRed->setLabel( "Output Red Mix" );

	OFX::RGBAParamDescriptor* outputGreen = desc.defineRGBAParam( kGreenMixedValues );
	outputGreen->setDefault( 0, 1, 0, 0 );
	outputGreen->setLabel( "Output Green Mix" );

	OFX::RGBAParamDescriptor* outputBlue = desc.defineRGBAParam( kBlueMixedValues );
	outputBlue->setDefault( 0, 0, 1, 0 );
	outputBlue->setLabel( "Output Blue Mix" );

	OFX::RGBAParamDescriptor* outputAlpha = desc.defineRGBAParam( kAlphaMixedValues );
	outputAlpha->setDefault( 0, 0, 0, 1 );
	outputAlpha->setLabel( "Output Alpha Mix" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ColorMatrixPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new ColorMatrixPlugin( handle );
}

}
}
}

