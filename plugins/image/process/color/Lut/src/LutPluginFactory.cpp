#include "LutPluginFactory.hpp"
#include "LutPlugin.hpp"
#include "LutDefinitions.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {
namespace lut {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void LutPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleLut", "Lut", "Color transformation through CLUT file" );
	desc.setPluginGrouping( "tuttle/image/process/color" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextGeneral );
	desc.addSupportedContext( OFX::eContextFilter );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	desc.setSupportsTiles( kSupportTiles );
	//desc.setRenderThreadSafety( OFX::eRenderFullySafe ); //< @todo tuttle: remove process data from LutPlugin
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void LutPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc, OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	// Controls
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kInputFilename );
	filename->setDefault( "" );
	filename->setLabels( kInputFilenameLabel, kInputFilenameLabel, kInputFilenameLabel );
	filename->setStringType( OFX::eStringTypeFilePath );

	OFX::PushButtonParamDescriptor* helpButton = desc.definePushButtonParam( kHelp );
	helpButton->setLabel( "Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* LutPluginFactory::createInstance( OfxImageEffectHandle handle, OFX::EContext context )
{
	return new LutPlugin( handle );
}

}
}
}
