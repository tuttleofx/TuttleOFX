#include "AVReaderPluginFactory.hpp"
#include "AVReaderPlugin.hpp"
#include "AVReaderDefinitions.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <string>
#include <vector>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void AVReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels(
		"TuttleAVReader",
		"AVReader",
		"Audio Video reader" );
	desc.setPluginGrouping( "tuttle/image/io" );

	std::vector<std::string> supportedExtensions;
	{
		AVInputFormat* iFormat = av_iformat_next( NULL );
		while( iFormat != NULL )
		{
			if( iFormat->extensions != NULL )
			{
				using namespace boost::algorithm;
				const std::string extStr( iFormat->extensions );
				std::vector<std::string> exts;
				split( exts, extStr, is_any_of(",") );
				supportedExtensions.insert( supportedExtensions.end(), exts.begin(), exts.end() );
			}
			iFormat = av_iformat_next( iFormat );
		}
	}
	
	desc.setDescription( "Video reader based on LibAV library\n\n"
			"Supported extensions: \n" +
			boost::algorithm::join( supportedExtensions, ", " )
		);
	
	// add the supported contexts
	desc.addSupportedContext( OFX::eContextReader );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// add supported extensions
	desc.addSupportedExtensions( supportedExtensions );
	
	// plugin flags
	desc.setRenderThreadSafety( OFX::eRenderInstanceSafe );
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
void AVReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                   OFX::EContext               context )
{
	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::BooleanParamDescriptor* useCustomSAR = desc.defineBooleanParam( kParamUseCustomSAR );
	useCustomSAR->setLabel( "Override SAR" );
	useCustomSAR->setDefault( false );
	useCustomSAR->setHint( "Override the file SAR (Storage Aspect Ratio) with a custom SAR value." );

	OFX::DoubleParamDescriptor* customSAR = desc.defineDoubleParam( kParamCustomSAR );
	customSAR->setLabel( "Custom SAR" );
	customSAR->setDefault( 1.0 );
	customSAR->setHint( "Choose a custom value to override the file SAR (Storage Aspect Ratio)." );

	describeReaderParamsInContext( desc, context );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* AVReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                         OFX::EContext        context )
{
	return new AVReaderPlugin( handle );
}

}
}
}
}
