/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#include "FftPluginFactory.hpp"
#include "FftPlugin.hpp"
#include "FftDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace fft {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void FftPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleFft", "Fft",
	                "Forward fft" );
	desc.setPluginGrouping( "tuttle/image/process/filter" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextGeneral );
	desc.addSupportedContext( OFX::eContextFilter );

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
void FftPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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

	OFX::ChoiceParamDescriptor* fftTypeLong = desc.defineChoiceParam( kFftTypeLong );
	fftTypeLong->setLabel( "fft type" );
	fftTypeLong->appendOption( "FFT Magnitude" );
	fftTypeLong->appendOption( "FFT Module" );          // 1st Input of inverse fft
	fftTypeLong->appendOption( "FFT Phase" );           // 2nd Input of inverse fft

	OFX::ChoiceParamDescriptor* fftType = desc.defineChoiceParam( kFftType );
	fftType->setIsSecret( true );
	fftType->appendOption( "FFT Magnitude" );
	fftType->appendOption( "FFT Module" );
	fftType->appendOption( "FFT Phase" );
	fftType->setDefault( 0 );

	OFX::PushButtonParamDescriptor* helpButton = desc.definePushButtonParam( kHelpButton );
	helpButton->setLabel( "Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* FftPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                    OFX::EContext        context )
{
	return new FftPlugin( handle );
}

}
}
}
}
