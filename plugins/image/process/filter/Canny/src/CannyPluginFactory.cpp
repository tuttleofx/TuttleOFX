#include "CannyPluginFactory.hpp"
#include "CannyPlugin.hpp"
#include "CannyDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace canny {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void CannyPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "Canny", "Canny",
		            "Canny" );
	desc.setPluginGrouping( "tuttle" );

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
void CannyPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::EContext context )
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

	OFX::BooleanParamDescriptor* hysteresisThresholding = desc.defineBooleanParam( kParamHysteresis );
	hysteresisThresholding->setLabel( "Hysteresis thresholding" );
	hysteresisThresholding->setDefault( true );

	OFX::DoubleParamDescriptor* upperThres = desc.defineDoubleParam( kParamUpperThres );
	upperThres->setLabel( "Upper thresold" );
	upperThres->setDefault( 0.5 );
	upperThres->setRange( 0.0, std::numeric_limits<double>::max() );
	upperThres->setDisplayRange( 0.0, 1.0 );

	OFX::DoubleParamDescriptor* lowerThres = desc.defineDoubleParam( kParamLowerThres );
	lowerThres->setLabel( "Lower thresold" );
	lowerThres->setDefault( 0.1 );
	lowerThres->setRange( 0.0, std::numeric_limits<double>::max() );
	lowerThres->setDisplayRange( 0.0, 1.0 );

	OFX::ChoiceParamDescriptor* border = desc.defineChoiceParam( kParamBorder );
	border->setLabel( "Border" );
//	border->setHint( "Border method." );
	border->appendOption( kParamBorderBlack );
	border->appendOption( kParamBorderPadded );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* CannyPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new CannyPlugin( handle );
}

}
}
}

