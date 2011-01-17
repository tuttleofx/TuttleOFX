#include "ImageMagickReaderPluginFactory.hpp"
#include "ImageMagickReaderDefinitions.hpp"
#include "ImageMagickReaderPlugin.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace imagemagick {
namespace reader {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void ImageMagickReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleImageMagickReader", "ImageMagickReader",
	                "ImageMagick file reader" );
	desc.setPluginGrouping( "tuttle/image/io" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextReader );
	desc.addSupportedContext( OFX::eContextGenerator );
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
void ImageMagickReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                        OFX::EContext               context )
{
	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
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
OFX::ImageEffect* ImageMagickReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                                  OFX::EContext        context )
{
	return new ImageMagickReaderPlugin( handle );
}

}
}
}
}
