#include "BlurPluginFactory.hpp"
#include "BlurPlugin.hpp"
#include "BlurDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <limits>

namespace tuttle {
namespace plugin {
namespace blur {

static const bool kSupportTiles = true;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void BlurPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleBlur", "TuttleBlur",
	                "TuttleBlur" );
	desc.setPluginGrouping( "tuttle/image/process/filter" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	//	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	//	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void BlurPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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

	OFX::Double2DParamDescriptor* size = desc.defineDouble2DParam( kParamSize );
	size->setLabel( "Size" );
	size->setDefault( 3, 3 );
	size->setRange( 0.0, 0.0, std::numeric_limits<double>::max(), std::numeric_limits<double>::max() );
	size->setDisplayRange( 0, 0, 10, 10 );
	size->setDoubleType( OFX::eDoubleTypeScale );

	OFX::ChoiceParamDescriptor* border = desc.defineChoiceParam( kParamBorder );
	border->setLabel( "Border" );
	border->appendOption( kParamBorderMirror );
	border->appendOption( kParamBorderConstant );
	border->appendOption( kParamBorderBlack );
	border->appendOption( kParamBorderPadded );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* BlurPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                     OFX::EContext        context )
{
	return new BlurPlugin( handle );
}

}
}
}

