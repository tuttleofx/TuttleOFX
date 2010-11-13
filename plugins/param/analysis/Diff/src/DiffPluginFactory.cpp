#include "DiffPluginFactory.hpp"
#include "DiffPlugin.hpp"
#include "DiffDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace quality {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void DiffPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleDiff", "TuttleDiff", "TuttleDiff" );
	desc.setPluginGrouping( "tuttle/param/analysis" );

	// add the supported contexts, only filter at the moment
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
void DiffPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                           OFX::EContext               context )
{
	OFX::ClipDescriptor* srcClipA = desc.defineClip( kDiffSourceA );

	assert( srcClipA );
	srcClipA->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClipA->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClipA->setSupportsTiles( kSupportTiles );
	srcClipA->setOptional( false );

	OFX::ClipDescriptor* srcClipB = desc.defineClip( kDiffSourceB );
	assert( srcClipB );
	srcClipB->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClipB->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClipB->setSupportsTiles( kSupportTiles );
	srcClipB->setOptional( false );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	assert( dstClip );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::ChoiceParamDescriptor* diffFunction = desc.defineChoiceParam( kMeasureFunction );
	assert( diffFunction );
	diffFunction->setLabel( "Quality mesure function" );
	diffFunction->appendOption( "PSNR (Peak Signal to Noise Ratio)" );
	diffFunction->appendOption( "MSE (Mean Square Error)" );
	diffFunction->appendOption( "SSIM (Structural SIMilarity)" );
	diffFunction->setDefault( 0 );

	OFX::RGBAParamDescriptor* outputQualityMesure = desc.defineRGBAParam( kOutputQualityMesure );
	assert( outputQualityMesure );
	outputQualityMesure->setLabel( "Quality" );
	outputQualityMesure->setEvaluateOnChange( false );

	OFX::PushButtonParamDescriptor* helpButton = desc.definePushButtonParam( kHelpButton );
	assert( helpButton );
	helpButton->setLabel( "Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* DiffPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                     OFX::EContext        context )
{
	return new DiffPlugin( handle );
}

}
}
}
