#include "DPXReaderDefinitions.hpp"
#include "DPXReaderPluginFactory.hpp"
#include "DPXReaderPlugin.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace reader {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void DPXReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleDpxReader", "DpxReader",
	                "Dpx file reader" );
	desc.setPluginGrouping( "tuttle/image/io" );

	desc.setDescription( "Digital Picture Exchange (DPX), ANSI/SMPTE standard (268M-2003)" );

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
void DPXReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                OFX::EContext               context )
{
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	// Dpx only supports RGB(A)
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->setSupportsTiles( kSupportTiles );

	describeReaderParamsInContext( desc, context );

	OFX::PushButtonParamDescriptor* displayHeader = desc.definePushButtonParam( kParamDisplayHeader );
	displayHeader->setLabel( "See Header" );
	displayHeader->setHint( "See the file header without formating (debug purpose only)." );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* DPXReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                          OFX::EContext        context )
{
	return new DPXReaderPlugin( handle );
}

}
}
}
}
