#include "TimeShiftPluginFactory.hpp"
#include "TimeShiftPlugin.hpp"
#include "TimeShiftDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace timeShift {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void TimeShiftPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleTimeShift", "TimeShift",
	                "TimeShift" );
	desc.setPluginGrouping( "tuttle/image/process/time" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextGeneral );
	desc.addSupportedContext( OFX::eContextFilter );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setTemporalClipAccess( true );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void TimeShiftPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                OFX::EContext               context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );

	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );

	OFX::DoubleParamDescriptor* offset = desc.defineDoubleParam( kOffset );
	offset->setLabel( "offset" );
	offset->setDefault( 0.0 );
	offset->setDisplayRange( -10, 10 );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* TimeShiftPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                          OFX::EContext        context )
{
	return new TimeShiftPlugin( handle );
}

}
}
}
