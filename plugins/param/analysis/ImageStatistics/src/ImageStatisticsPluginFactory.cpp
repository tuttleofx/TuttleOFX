#include "ImageStatisticsPluginFactory.hpp"
#include "ImageStatisticsPlugin.hpp"
#include "ImageStatisticsDefinitions.hpp"
#include "ImageStatisticsOverlayInteract.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace imageStatistics {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void ImageStatisticsPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleImageStatistics", "ImageStatistics",
	                "Image statistics" );
	desc.setPluginGrouping( "tuttle/param/analysis" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextGeneral );
	desc.addSupportedContext( OFX::eContextFilter );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsMultiResolution( false );
	desc.setSupportsTiles( kSupportTiles );

	desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayWrap<ImageStatisticsEffectOverlayDescriptor>() );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ImageStatisticsPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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

	OFX::ChoiceParamDescriptor* coordSystem = desc.defineChoiceParam( kParamCoordinateSystem );
	coordSystem->setLabel( "Coordinate system" );
	coordSystem->appendOption( kParamCoordinateSystemNormalized );
	coordSystem->appendOption( kParamCoordinateSystemCanonical );
	coordSystem->setDefault( 0 );

	OFX::Double2DParamDescriptor* rectCenter = desc.defineDouble2DParam( kParamRectCenter );
	rectCenter->setLabel( "Center" );
	rectCenter->setDoubleType( OFX::eDoubleTypePlain );
	//	rectCenter->setDoubleType( OFX::eDoubleTypeNormalisedXYAbsolute );
	rectCenter->setDefault( 0.5, 0.5 );

	OFX::Double2DParamDescriptor* rectSize = desc.defineDouble2DParam( kParamRectSize );
	rectSize->setLabel( "Size" );
	rectSize->setDoubleType( OFX::eDoubleTypePlain );
	//	rectSize->setDoubleType( OFX::eDoubleTypeNormalisedXYAbsolute );
	rectSize->setDefault( 0.5, 0.5 );

	OFX::ChoiceParamDescriptor* chooseOutput = desc.defineChoiceParam( kParamChooseOutput );
	chooseOutput->setLabel( "Choose output" );
	chooseOutput->appendOption( kParamChooseOutputSource );
	chooseOutput->appendOption( kParamChooseOutputAverage );
	chooseOutput->appendOption( kParamChooseOutputChannelMin );
	chooseOutput->appendOption( kParamChooseOutputChannelMax );
	chooseOutput->appendOption( kParamChooseOutputLuminosityMin );
	chooseOutput->appendOption( kParamChooseOutputLuminosityMax );
	chooseOutput->setDefault( 0 );

	OFX::GroupParamDescriptor* outputGroup = desc.defineGroupParam( kParamOutputGroup );
	outputGroup->setLabel( "Output" );

	// -----------------------------------------------------------------------------

	OFX::GroupParamDescriptor* rgbaGroup = desc.defineGroupParam( kParamOutputGroupRGBA );
	rgbaGroup->setLabel( "RGBA" );
	rgbaGroup->setParent( outputGroup );

	OFX::RGBAParamDescriptor* outputAverage = desc.defineRGBAParam( kParamOutputAverage );
	outputAverage->setLabel( "Average" );
	outputAverage->setParent( rgbaGroup );
	outputAverage->setEvaluateOnChange( false );

	OFX::RGBAParamDescriptor* outputChannelMin = desc.defineRGBAParam( kParamOutputChannelMin );
	outputChannelMin->setLabel( "Channels' min" );
	outputChannelMin->setHint( "Minimum value per channel" );
	outputChannelMin->setParent( rgbaGroup );
	outputChannelMin->setEvaluateOnChange( false );

	OFX::RGBAParamDescriptor* outputChannelMax = desc.defineRGBAParam( kParamOutputChannelMax );
	outputChannelMax->setLabel( "Channels' max" );
	outputChannelMax->setParent( rgbaGroup );
	outputChannelMax->setEvaluateOnChange( false );

	OFX::RGBAParamDescriptor* outputLuminosityMin = desc.defineRGBAParam( kParamOutputLuminosityMin );
	outputLuminosityMin->setLabel( "Luminosity min" );
	outputLuminosityMin->setParent( rgbaGroup );
	outputLuminosityMin->setEvaluateOnChange( false );

	OFX::RGBAParamDescriptor* outputLuminosityMax = desc.defineRGBAParam( kParamOutputLuminosityMax );
	outputLuminosityMax->setLabel( "Luminosity max" );
	outputLuminosityMax->setParent( rgbaGroup );
	outputLuminosityMax->setEvaluateOnChange( false );

	OFX::RGBAParamDescriptor* outputKurtosis = desc.defineRGBAParam( kParamOutputKurtosis );
	outputKurtosis->setLabel( "Kurtosis" );
	outputKurtosis->setParent( rgbaGroup );
	outputKurtosis->setEvaluateOnChange( false );

	OFX::RGBAParamDescriptor* outputSkewness = desc.defineRGBAParam( kParamOutputSkewness );
	outputSkewness->setLabel( "Skewness" );
	outputSkewness->setParent( rgbaGroup );
	outputSkewness->setEvaluateOnChange( false );

	// -----------------------------------------------------------------------------

	OFX::GroupParamDescriptor* hslGroup = desc.defineGroupParam( kParamOutputGroupHSL );
	hslGroup->setLabel( "HSL" );
	hslGroup->setParent( outputGroup );

	OFX::Double3DParamDescriptor* outputAverageHSL = desc.defineDouble3DParam( kParamOutputAverageHSL );
	outputAverageHSL->setLabel( "Average" );
	outputAverageHSL->setDoubleType( OFX::eDoubleTypePlain );
	outputAverageHSL->setDimensionLabels( "h", "s", "l" );
	outputAverageHSL->setParent( hslGroup );
	outputAverageHSL->setEvaluateOnChange( false );

	OFX::Double3DParamDescriptor* outputChannelMinHSL = desc.defineDouble3DParam( kParamOutputChannelMinHSL );
	outputChannelMinHSL->setLabel( "Channels' min" );
	outputChannelMinHSL->setHint( "Minimum value per channel" );
	outputChannelMinHSL->setDoubleType( OFX::eDoubleTypePlain );
	outputChannelMinHSL->setDimensionLabels( "h", "s", "l" );
	outputChannelMinHSL->setParent( hslGroup );
	outputChannelMinHSL->setEvaluateOnChange( false );

	OFX::Double3DParamDescriptor* outputChannelMaxHSL = desc.defineDouble3DParam( kParamOutputChannelMaxHSL );
	outputChannelMaxHSL->setLabel( "Channels' max" );
	outputChannelMaxHSL->setDoubleType( OFX::eDoubleTypePlain );
	outputChannelMaxHSL->setDimensionLabels( "h", "s", "l" );
	outputChannelMaxHSL->setParent( hslGroup );
	outputChannelMaxHSL->setEvaluateOnChange( false );

	OFX::Double3DParamDescriptor* outputLuminosityMinHSL = desc.defineDouble3DParam( kParamOutputLuminosityMinHSL );
	outputLuminosityMinHSL->setLabel( "Luminosity min" );
	outputLuminosityMinHSL->setDoubleType( OFX::eDoubleTypePlain );
	outputLuminosityMinHSL->setDimensionLabels( "h", "s", "l" );
	outputLuminosityMinHSL->setParent( hslGroup );
	outputLuminosityMinHSL->setEvaluateOnChange( false );

	OFX::Double3DParamDescriptor* outputLuminosityMaxHSL = desc.defineDouble3DParam( kParamOutputLuminosityMaxHSL );
	outputLuminosityMaxHSL->setLabel( "Luminosity max" );
	outputLuminosityMaxHSL->setDoubleType( OFX::eDoubleTypePlain );
	outputLuminosityMaxHSL->setDimensionLabels( "h", "s", "l" );
	outputLuminosityMaxHSL->setParent( hslGroup );
	outputLuminosityMaxHSL->setEvaluateOnChange( false );

	OFX::Double3DParamDescriptor* outputKurtosisHSL = desc.defineDouble3DParam( kParamOutputKurtosisHSL );
	outputKurtosisHSL->setLabel( "Kurtosis" );
	outputKurtosisHSL->setDoubleType( OFX::eDoubleTypePlain );
	outputKurtosisHSL->setDimensionLabels( "h", "s", "l" );
	outputKurtosisHSL->setParent( hslGroup );
	outputKurtosisHSL->setEvaluateOnChange( false );

	OFX::Double3DParamDescriptor* outputSkewnessHSL = desc.defineDouble3DParam( kParamOutputSkewnessHSL );
	outputSkewnessHSL->setLabel( "Skewness" );
	outputSkewnessHSL->setDoubleType( OFX::eDoubleTypePlain );
	outputSkewnessHSL->setDimensionLabels( "h", "s", "l" );
	outputSkewnessHSL->setParent( hslGroup );
	outputSkewnessHSL->setEvaluateOnChange( false );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ImageStatisticsPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                                OFX::EContext        context )
{
	return new ImageStatisticsPlugin( handle );
}

}
}
}
