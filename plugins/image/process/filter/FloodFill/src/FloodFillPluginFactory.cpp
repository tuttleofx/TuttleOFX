#include "FloodFillPluginFactory.hpp"
#include "FloodFillPlugin.hpp"
#include "FloodFillDefinitions.hpp"

#include <ofxImageEffect.h>

#include <limits>

namespace tuttle {
namespace plugin {
namespace floodFill {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void FloodFillPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleFloodFill", "FloodFill",
		            "FloodFill" );
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
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void FloodFillPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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
	
	OFX::DoubleParamDescriptor* upperThres = desc.defineDoubleParam( kParamUpperThres );
	upperThres->setLabel( "Upper thresold" );
	upperThres->setDefault( 0.1 );
	upperThres->setRange( 0.0, std::numeric_limits<double>::max() );
	upperThres->setDisplayRange( 0.0, 1.0 );

	OFX::DoubleParamDescriptor* lowerThres = desc.defineDoubleParam( kParamLowerThres );
	lowerThres->setLabel( "Lower thresold" );
	lowerThres->setDefault( 0.025 );
	lowerThres->setRange( 0.0, std::numeric_limits<double>::max() );
	lowerThres->setDisplayRange( 0.0, 1.0 );

	OFX::BooleanParamDescriptor* minmax = desc.defineBooleanParam( kParamMinMaxRelative );
	minmax->setLabel( "Relative to min/max" );
	minmax->setHint( "Use theshold values relative to min/max" );
	minmax->setDefault( true );

	OFX::ChoiceParamDescriptor* method = desc.defineChoiceParam( kParamMethod );
	method->setLabel( "Method" );
	method->appendOption( kParamMethod4Connections );
	method->appendOption( kParamMethod8Connections );
#ifndef TUTTLE_PRODUCTION
	method->appendOption( kParamMethodBruteForce );
#endif
	method->setDefault( 1 );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* FloodFillPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new FloodFillPlugin( handle );
}

}
}
}

