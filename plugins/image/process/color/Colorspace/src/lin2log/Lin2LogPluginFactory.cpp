#include "Lin2LogPluginFactory.hpp"
#include "Lin2LogPlugin.hpp"
#include "Lin2LogDefinitions.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {
namespace colorspace {
namespace lin2log {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void Lin2LogPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	// basic labels
	desc.setLabels( "TuttleLin2log", "Lin2log",
	                "Linear to logarithmic convertion" );
	desc.setPluginGrouping( "tuttle/image/process/color" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void Lin2LogPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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

	OFX::PushButtonParamDescriptor* helpButton = desc.definePushButtonParam( kLin2LogHelpButton );
	helpButton->setScriptName( "&Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* Lin2LogPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                        OFX::EContext        context )
{
	return new Lin2LogPlugin( handle );
}

}
}
}
}
