#include "ConvolutionPluginFactory.hpp"
#include "ConvolutionPlugin.hpp"
#include "ConvolutionDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace convolution {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ConvolutionPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleConvolution", "Convolution",
	                "Convolution" );
	desc.setPluginGrouping( "tuttle/image/process/filter" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ConvolutionPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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

	OFX::Int2DParamDescriptor* size = desc.defineInt2DParam( kParamSize );
	size->setLabel( "Size" );
	size->setDefault( 3, 3 );
	//	size->setIncrement( 2, 2 );
	size->setRange( 3, 3, kParamSizeMax, kParamSizeMax );

	for( unsigned int y = 0; y < kParamSizeMax; ++y )
	{
		for( unsigned int x = 0; x < kParamSizeMax; ++x )
		{
			const std::string name( getCoefName( y, x ) );
			OFX::DoubleParamDescriptor* coef = desc.defineDoubleParam( name );
			coef->setLabel( name );
			coef->setDisplayRange( -10.0, 10.0 );
			coef->setDefault( 0.0 );
		}
	}

	OFX::PushButtonParamDescriptor* helpButton = desc.definePushButtonParam( kHelpButton );
	helpButton->setLabel( "Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ConvolutionPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext        context )
{
	return new ConvolutionPlugin( handle );
}

}
}
}
