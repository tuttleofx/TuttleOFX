#include "PrintPluginFactory.hpp"
#include "PrintPlugin.hpp"
#include "PrintDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle {
namespace plugin {
namespace print {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void PrintPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttlePrint", "TuttlePrint",
		            "TuttlePrint" );
	desc.setPluginGrouping( "tuttle" );

	desc.setDescription( "TuttlePrint\n"
	                     "Allows to print a part of the image." );

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
void PrintPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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

	OFX::ChoiceParamDescriptor* mode = desc.defineChoiceParam( kParamMode );
	mode->setLabel( "Mode" );
	mode->appendOption( kParamModeImage );
	mode->appendOption( kParamModeRegion );
	mode->appendOption( kParamModePixel );

	OFX::Int2DParamDescriptor* pixel = desc.defineInt2DParam( kParamPixel );
	pixel->setLabel( "Pixel" );
	pixel->setDisplayRange( 0, 0, 2000, 2000 );

	OFX::Int2DParamDescriptor* regionMin = desc.defineInt2DParam( kParamRegionMin );
	regionMin->setLabel( "Region min" );
	regionMin->setDisplayRange( 0, 0, 2000, 2000 );

	OFX::Int2DParamDescriptor* regionMax = desc.defineInt2DParam( kParamRegionMax );
	regionMax->setLabel( "Region max" );
	regionMax->setDisplayRange( 0, 0, 2000, 2000 );

	OFX::ChoiceParamDescriptor* output = desc.defineChoiceParam( kParamOutput );
	output->setLabel( "Output" );
	output->appendOption( kParamOutputNumeric );
	output->appendOption( kParamOutputAscii );

	OFX::BooleanParamDescriptor* flip = desc.defineBooleanParam( kParamFlip );
	flip->setLabel( "Flip" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* PrintPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new PrintPlugin( handle );
}

}
}
}

