#include "LocalMaximaPluginFactory.hpp"
#include "LocalMaximaPlugin.hpp"
#include "LocalMaximaDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace localmaxima {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void LocalMaximaPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleLocalMaxima", "LocalMaxima",
		            "LocalMaxima" );
	desc.setPluginGrouping( "tuttle/image/process/filter" );

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
	desc.setHostFrameThreading( false );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void LocalMaximaPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->setSupportsTiles( kSupportTiles );

	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::ChoiceParamDescriptor* border = desc.defineChoiceParam( kParamBorder );
	border->setLabel( "Border" );
//	border->setHint( "Border method." );
	border->appendOption( kParamBorderBlack );
//	border->appendOption( kParamBorderPadded );
	border->setDefault( 0 );

	OFX::ChoiceParamDescriptor* outputComponent = desc.defineChoiceParam( kParamOutputComponent );
	outputComponent->setLabel( "Output component" );
	outputComponent->appendOption( OFX::getImageEffectHostDescription()->supportsPixelComponent(OFX::ePixelComponentRGBA) ? kParamOutputComponentRGBA : "---" );
	outputComponent->appendOption( OFX::getImageEffectHostDescription()->supportsPixelComponent(OFX::ePixelComponentRGB) ? kParamOutputComponentRGB : "---" );
	outputComponent->appendOption( OFX::getImageEffectHostDescription()->supportsPixelComponent(OFX::ePixelComponentAlpha) ? kParamOutputComponentAlpha : "---" );
	outputComponent->setIsSecret( OFX::getImageEffectHostDescription()->_supportedComponents.size() == 1 );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* LocalMaximaPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new LocalMaximaPlugin( handle );
}

}
}
}

