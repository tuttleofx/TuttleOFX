#include "JpegReaderPluginFactory.hpp"
#include "JpegReaderDefinitions.hpp"
#include "JpegReaderPlugin.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace reader {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void JpegReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleJpegReader", "JpegReader",
		    "Jpeg file reader" );
	desc.setPluginGrouping( "tuttle/image/io" );

	std::vector<std::string> extension;
	extension.push_back( "jpeg" );
	extension.push_back( "jpg" );
	extension.push_back( "jpe" );
	extension.push_back( "jfif" );
	extension.push_back( "jfi" );

	std::string listOfExt;
	for( unsigned int i=0; i< extension.size(); i++ )
	{
		listOfExt += extension.at(i);
		listOfExt += ", ";
	}
	listOfExt.erase( listOfExt.size()-2, 2 );

	desc.setDescription( "JPEG File reader\n"
			 "Plugin is used to read jpeg files.\n\n"
			 "supported extensions: \n" +
			 listOfExt
	);

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextReader );
	desc.addSupportedContext( OFX::eContextGenerator );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );

	// add supported extensions
	for( unsigned int i=0; i< extension.size(); i++ )
	{
		desc.addSupportedExtension( extension.at(i) );
	}


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
void JpegReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
						 OFX::EContext               context )
{
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->setSupportsTiles( kSupportTiles );

	describeReaderParamsInContext( desc, context );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* JpegReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
							   OFX::EContext        context )
{
	return new JpegReaderPlugin( handle );
}

}
}
}
}
