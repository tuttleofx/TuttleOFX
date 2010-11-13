#include "ColorDistributionPluginFactory.hpp"
#include "ColorDistributionPlugin.hpp"
#include "ColorDistributionDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace colorDistribution {

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ColorDistributionPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleColorDistribution", "ColorDistribution",
	                "ColorDistribution" );
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
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ColorDistributionPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                        OFX::EContext               context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );

	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::ChoiceParamDescriptor* in = desc.defineChoiceParam( kParamIn );
	in->setLabel( "In" );
	in->setHint( "Input color distribution." );
	in->appendOption( kParamDistribution_linear );
	in->appendOption( kParamDistribution_sRGB );
	//	in->appendOption( kParamDistribution_cineon );
	//	in->appendOption( kParamDistribution_rec709 );
	//	in->appendOption( kParamDistribution_rec601 );
	in->setDefault( 0 );

	OFX::ChoiceParamDescriptor* out = desc.defineChoiceParam( kParamOut );
	out->setLabel( "Out" );
	out->setHint( "Output color distribution." );
	out->appendOption( kParamDistribution_linear );
	out->appendOption( kParamDistribution_sRGB );
	//	out->appendOption( kParamDistribution_cineon );
	//	out->appendOption( kParamDistribution_rec709 );
	//	out->appendOption( kParamDistribution_rec601 );
	out->setDefault( 0 );

	OFX::PushButtonParamDescriptor* invert = desc.definePushButtonParam( kParamInvert );
	invert->setLabel( "Invert" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ColorDistributionPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                                  OFX::EContext        context )
{
	return new ColorDistributionPlugin( handle );
}

}
}
}

