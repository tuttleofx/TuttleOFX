#include "BasicKeyerPluginFactory.hpp"
#include "BasicKeyerPlugin.hpp"
#include "BasicKeyerDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace basicKeyer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void BasicKeyerPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleBasicKeyer", "BasicKeyer",
	                "Basic keyer" );
	desc.setPluginGrouping( "tuttle/image/process/color" );

	// add the supported contexts
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
void BasicKeyerPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                 OFX::EContext               context )
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

	OFX::ChoiceParamDescriptor* mode = desc.defineChoiceParam( kParamMode );
	mode->appendOption( kParamModePosition );
	mode->appendOption( kParamModeColor );
	mode->setDefault( 0 );

	OFX::IntParamDescriptor* nbPoint = desc.defineIntParam( kParamNbPoints );
	nbPoint->setRange( 1, kMaxNbPoints );
	nbPoint->setDisplayRange( 1, 10 );
	nbPoint->setDefault( 1 );

	for( unsigned int i = 0; i < kMaxNbPoints; ++i )
	{
		OFX::Double2DParamDescriptor* point = desc.defineDouble2DParam( getPointParamName( i ) );
		point->setLabel( getPointParamName( i ) );
		//		point->setIsSecret( true );
		point->setDoubleType( OFX::eDoubleTypeNormalisedXYAbsolute );
		OFX::RGBAParamDescriptor* color = desc.defineRGBAParam( getColorParamName( i ) );
		color->setLabel( getColorParamName( i ) );
		//		color->setIsSecret( true );
	}
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* BasicKeyerPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                           OFX::EContext        context )
{
	return new BasicKeyerPlugin( handle );
}

}
}
}
