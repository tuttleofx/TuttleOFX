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
	desc.setLabels( "TuttleBlur", "Blur",
	                "Blur" );
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
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
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
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
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


	OFX::GroupParamDescriptor* advanced = desc.defineGroupParam( kParamGroupAdvanced );
	advanced->setLabel( "Advanced" );

	OFX::BooleanParamDescriptor* normalizedKernel = desc.defineBooleanParam( kParamNormalizedKernel );
	normalizedKernel->setLabel( "Normalized kernel" );
	normalizedKernel->setHint( "Use a normalized kernel to compute the gradient." );
	normalizedKernel->setDefault( true );
	normalizedKernel->setParent( advanced );

	OFX::DoubleParamDescriptor* kernelEpsilon = desc.defineDoubleParam( kParamKernelEpsilon );
	kernelEpsilon->setLabel( "Kernel espilon value" );
	kernelEpsilon->setHint( "Threshold at which we no longer consider the values of the function." );
	kernelEpsilon->setDefault( 0.01 );
	kernelEpsilon->setDisplayRange( 0, 0.01 );
	kernelEpsilon->setParent( advanced );
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

