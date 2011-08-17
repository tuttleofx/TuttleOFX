#include "Jpeg2000ReaderPluginFactory.hpp"
#include "Jpeg2000ReaderPlugin.hpp"
#include "Jpeg2000ReaderDefinitions.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace reader {


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void Jpeg2000ReaderPluginFactory::describe( OFX::ImageEffectDescriptor &desc )
{
	desc.setLabels(
		"TuttleJpeg2000Reader",
		"Jpeg2000Reader",
		"Jpeg2000 image reader" );
	desc.setPluginGrouping( "tuttle/image/io" );

	desc.setDescription(
		"Jpeg2000 io.\n"
	    "Plugin is used to read jpeg 2000 files."
		);

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextReader );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
	desc.setHostFrameThreading( false );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsMultipleClipDepths( true );
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void Jpeg2000ReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor &desc,
                                                   OFX::EContext context )
{
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
//	srcClip->addSupportedComponent( OFX::ePixelComponentRGB ); /// @todo: support RGB
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	describeReaderParamsInContext( desc, context );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* Jpeg2000ReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new Jpeg2000ReaderPlugin(handle);
}

}
}
}
}
