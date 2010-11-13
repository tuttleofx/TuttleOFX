#include "DebugImageEffectApiPluginFactory.hpp"
#include "DebugImageEffectApiPlugin.hpp"
#include "DebugImageEffectApiDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace debugImageEffectApi {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void DebugImageEffectApiPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleDebugImageEffectApi", "DebugImageEffectApi",
	                "Plugin which displays informations about the host and all plugin inputs." );
	desc.setPluginGrouping( "tuttle/param/debug" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSingleInstance( false );
	desc.setHostFrameThreading( true );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsTiles( kSupportTiles );
	desc.setTemporalClipAccess( kSupportTemporalClipAccess );
	desc.setRenderTwiceAlways( false );
	desc.setSupportsMultipleClipPARs( false );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void DebugImageEffectApiPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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

	OFX::StringParamDescriptor* hostInfos = desc.defineStringParam( kHostInfos );
	hostInfos->setLabel( "Host infos" );
	hostInfos->setStringType( OFX::eStringTypeMultiLine );
	hostInfos->setEvaluateOnChange( false );

	OFX::DoubleParamDescriptor* currentTime = desc.defineDoubleParam( kCurrentTime );
	currentTime->setLabel( "Current time" );
	currentTime->setDefault( 0 );
	currentTime->setEvaluateOnChange( false );

	OFX::DoubleParamDescriptor* beginTime = desc.defineDoubleParam( kBeginTime );
	beginTime->setLabel( "Begin time" );
	beginTime->setDefault( 0 );
	beginTime->setEvaluateOnChange( false );

	OFX::DoubleParamDescriptor* endTime = desc.defineDoubleParam( kEndTime );
	endTime->setLabel( "End time" );
	endTime->setDefault( 0 );
	endTime->setEvaluateOnChange( false );

	OFX::PushButtonParamDescriptor* goTo = desc.definePushButtonParam( kGoTo );
	goTo->setLabel( "Go to" );

	OFX::PushButtonParamDescriptor* update = desc.definePushButtonParam( kUpdate );
	update->setLabel( "Update" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* DebugImageEffectApiPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                                    OFX::EContext        context )
{
	return new DebugImageEffectApiPlugin( handle );
}

}
}
}
