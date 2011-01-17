#include "ResizePluginFactory.hpp"
#include "ResizePlugin.hpp"
#include "ResizeDefinitions.hpp"

#include <limits>

namespace tuttle {
namespace plugin {
namespace resize {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ResizePluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleResize", "Resize",
		            "Resize" );
	desc.setPluginGrouping( "tuttle/image/process/geometry" );

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
void ResizePluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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

	OFX::Double2DParamDescriptor* size = desc.defineDouble2DParam( kParamSize );
	size->setDefault( 720, 576 );
	size->setLabel( "Size" );
//
//	OFX::Double2DParamDescriptor* scale = desc.defineDouble2DParam( kParamScale );
//	scale->setDefault( 1.0, 1.0 );
//	scale->setLabel( "Scale" );

}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ResizePluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new ResizePlugin( handle );
}

}
}
}

