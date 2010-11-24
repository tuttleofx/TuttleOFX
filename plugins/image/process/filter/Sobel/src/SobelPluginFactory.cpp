#include "SobelPluginFactory.hpp"
#include "SobelPlugin.hpp"
#include "SobelDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace sobel {

static const bool kSupportTiles = true;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void SobelPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "Sobel", "Sobel",
		            "Sobel" );
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
void SobelPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
//	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::Double2DParamDescriptor* size = desc.defineDouble2DParam( kParamSize );
	size->setLabel( "Size" );
	size->setDefault( 1.0, 1.0 );
	size->setRange( 0.0, 0.0, std::numeric_limits<double>::max(), std::numeric_limits<double>::max() );
	size->setDisplayRange( 0, 0, 10, 10 );
	size->setDoubleType( OFX::eDoubleTypeScale );

	OFX::BooleanParamDescriptor* unidimensional = desc.defineBooleanParam( kParamUnidimensional );
	unidimensional->setLabel( "Unidimensional" );
	unidimensional->setHint( "Instead of using a square convolution matrix, use 1D kernels." );
	unidimensional->setDefault( false );

	OFX::BooleanParamDescriptor* normalizedKernel = desc.defineBooleanParam( kParamNormalizedKernel );
	normalizedKernel->setLabel( "Normalized kernel" );
	normalizedKernel->setHint( "Use a normalized kernel to compute the gradient." );
	normalizedKernel->setDefault( true );

	OFX::ChoiceParamDescriptor* border = desc.defineChoiceParam( kParamBorder );
	border->setLabel( "Gradient border" );
	border->setHint( "Border method for gradient computation." );
	border->appendOption( kParamBorderMirror );
	border->appendOption( kParamBorderConstant );
	border->appendOption( kParamBorderBlack );
	border->appendOption( kParamBorderPadded );

	OFX::BooleanParamDescriptor* computeNorm = desc.defineBooleanParam( kParamComputeGradientNorm );
	computeNorm->setLabel( "Compute norm" );
	computeNorm->setHint( "To disable the norm computation, if you don't need it." );
	computeNorm->setDefault( true );

	OFX::BooleanParamDescriptor* normManhattan = desc.defineBooleanParam( kParamGradientNormManhattan );
	normManhattan->setLabel( "Use the manhattan norm" );
	normManhattan->setHint( "Use manhattan norm instead of standard one." );
	normManhattan->setDefault( false );

	OFX::BooleanParamDescriptor* computeGradientDirection = desc.defineBooleanParam( kParamComputeGradientDirection );
	computeGradientDirection->setLabel( "Gradient direction" );
	computeGradientDirection->setHint( "To disable the gradient direction computation, if you don't need it." );
	computeGradientDirection->setDefault( true );

	OFX::BooleanParamDescriptor* gradientDirectionAbs = desc.defineBooleanParam( kParamGradientDirectionAbs );
	gradientDirectionAbs->setLabel( "Angle between 0 and PI" );
	gradientDirectionAbs->setHint( "Limit gradient direction between 0 and PI." );
	gradientDirectionAbs->setDefault( true );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* SobelPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new SobelPlugin( handle );
}

}
}
}

