#include "ImageStatisticsPluginFactory.hpp"
#include "ImageStatisticsPlugin.hpp"
#include "ImageStatisticsDefinitions.hpp"
#include "ImageStatisticsOverlayInteract.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace imageStatistics {

static const std::string hintAverage =
	"Arithmetic mean\n"
	"The measure of central tendency of a set of values computed by dividing "
	"the sum of the values by their number; commonly called the mean or the average.";

static const std::string hintVariance =
	"Variance measures how far a set of numbers is spread out.\n"
	"A variance of zero indicates that all the values are identical. "
	"Variance is always non-negative: a small variance indicates that the data tend to be very "
	"close to the mean (expected value) and hence to each other, while a high variance indicates "
	"that the data are very spread out around the mean and from each other.";

static const std::string hintKurtosis =
	"Kurtosis is a descriptor of the shape of a probability distribution.\n"
	"Kurtosis is any measure of the 'peakedness' of the probability distribution "
	"of a real-valued random variable.\n";

static const std::string hintSkewness =
	"Skewness is a measure of the asymmetry of the probability distribution of "
	"a real-valued random variable about its mean. The skewness value can be "
	"positive or negative, or even undefined.";

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void ImageStatisticsPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleImageStatistics", "ImageStatistics",
	                "Image statistics" );
	desc.setPluginGrouping( "tuttle/param/analysis" );

	desc.setDescription( "ImageStatistics\n"
			 "Plugin is used to get statistics (about color, luminosity...) on a specific part of the given image." );

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

	OFX::ClipDescriptor* maskClip = desc.defineClip( kClipMask );
	maskClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	maskClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	maskClip->setSupportsTiles( kSupportTiles );
	maskClip->setIsMask(true);
	maskClip->setOptional(true);

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::ChoiceParamDescriptor* chooseOutput = desc.defineChoiceParam( kParamChooseOutput );
	chooseOutput->setLabel( "Choose output" );
	chooseOutput->appendOption( kParamChooseOutputSource );
	chooseOutput->appendOption( kParamChooseOutputAverage );
	chooseOutput->appendOption( kParamChooseOutputVariance );
	chooseOutput->appendOption( kParamChooseOutputChannelMin );
	chooseOutput->appendOption( kParamChooseOutputChannelMax );
	chooseOutput->appendOption( kParamChooseOutputLuminosityMin );
	chooseOutput->appendOption( kParamChooseOutputLuminosityMax );
	chooseOutput->setDefault( 0 );

	OFX::GroupParamDescriptor* selectRegionGroup = desc.defineGroupParam( kParamSelectRegionGroup );
	selectRegionGroup->setOpen( false );
	selectRegionGroup->setLabel( "Select Custom Region" );

	OFX::BooleanParamDescriptor* restrictToRegion = desc.defineBooleanParam( kParamRestrictToRegion );
	restrictToRegion->setLabel( "Restrict To Custom Region" );
	restrictToRegion->setDefault( false );
	restrictToRegion->setParent( selectRegionGroup );

	OFX::ChoiceParamDescriptor* coordSystem = desc.defineChoiceParam( kParamCoordinateSystem );
	coordSystem->setLabel( "Coordinate system" );
	coordSystem->appendOption( kParamCoordinateSystemNormalized );
	coordSystem->appendOption( kParamCoordinateSystemCanonical );
	coordSystem->setDefault( 0 );
	coordSystem->setParent(selectRegionGroup);

	OFX::Double2DParamDescriptor* rectCenter = desc.defineDouble2DParam( kParamRectCenter );
	rectCenter->setLabel( "Center" );
	rectCenter->setDoubleType( OFX::eDoubleTypePlain );
	//	rectCenter->setDoubleType( OFX::eDoubleTypeNormalisedXYAbsolute );
	rectCenter->setDefault( 0.5, 0.5 );
	rectCenter->setParent(selectRegionGroup);

	OFX::Double2DParamDescriptor* rectSize = desc.defineDouble2DParam( kParamRectSize );
	rectSize->setLabel( "Size" );
	rectSize->setDoubleType( OFX::eDoubleTypePlain );
	//	rectSize->setDoubleType( OFX::eDoubleTypeNormalisedXYAbsolute );
	rectSize->setDefault( 0.5, 0.5 );
	rectSize->setParent(selectRegionGroup);
	
	// -----------------------------------------------------------------------------
	
	OFX::GroupParamDescriptor* outputGroup = desc.defineGroupParam( kParamOutputGroup );
	outputGroup->setLabel( "Output" );
	outputGroup->setAsTab();
	
	OFX::IntParamDescriptor* outputNbPixels = desc.defineIntParam( kParamOutputNbPixels );
	outputNbPixels->setLabel( "Nb Pixels" );
	outputNbPixels->setHint(
		"Number of pixels used to compute statistics.\n"
		"It depends on the number of non 0 values in the input mask and "
		"the intersection between: input image RoD, input mask RoD and "
		"user defined region." );
	outputNbPixels->setEvaluateOnChange( false );
	outputNbPixels->setParent( outputGroup );

	// -----------------------------------------------------------------------------

	OFX::GroupParamDescriptor* rgbaGroup = desc.defineGroupParam( kParamOutputGroupRGBA );
	rgbaGroup->setLabel( "RGBA" );
	rgbaGroup->setParent( outputGroup );
	rgbaGroup->setAsTab();

	OFX::RGBAParamDescriptor* outputAverage = desc.defineRGBAParam( kParamOutputAverage );
	outputAverage->setLabel( "Average" );
	outputAverage->setHint( hintAverage );
	outputAverage->setParent( rgbaGroup );
	outputAverage->setEvaluateOnChange( false );
	
	OFX::RGBAParamDescriptor* outputVariance = desc.defineRGBAParam( kParamOutputVariance );
	outputVariance->setLabel( "Variance" );
	outputVariance->setHint( hintVariance );
	outputVariance->setParent( rgbaGroup );
	outputVariance->setEvaluateOnChange( false );
	
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
	outputKurtosis->setHint( hintKurtosis );
	outputKurtosis->setParent( rgbaGroup );
	outputKurtosis->setEvaluateOnChange( false );

	OFX::RGBAParamDescriptor* outputSkewness = desc.defineRGBAParam( kParamOutputSkewness );
	outputSkewness->setLabel( "Skewness" );
	outputSkewness->setHint( hintSkewness );
	outputSkewness->setParent( rgbaGroup );
	outputSkewness->setEvaluateOnChange( false );

	// -----------------------------------------------------------------------------

	OFX::GroupParamDescriptor* hslGroup = desc.defineGroupParam( kParamOutputGroupHSL );
	hslGroup->setLabel( "HSL" );
	hslGroup->setParent( outputGroup );
	hslGroup->setAsTab();

	OFX::Double3DParamDescriptor* outputAverageHSL = desc.defineDouble3DParam( kParamOutputAverageHSL );
	outputAverageHSL->setLabel( "Average" );
	outputAverageHSL->setHint( hintAverage );
	outputAverageHSL->setDoubleType( OFX::eDoubleTypePlain );
	outputAverageHSL->setDimensionLabels( "h", "s", "l" );
	outputAverageHSL->setParent( hslGroup );
	outputAverageHSL->setEvaluateOnChange( false );

	OFX::Double3DParamDescriptor* outputVarianceHSL = desc.defineDouble3DParam( kParamOutputVarianceHSL );
	outputVarianceHSL->setLabel( "Variance" );
	outputVarianceHSL->setHint( hintVariance );
	outputVarianceHSL->setDoubleType( OFX::eDoubleTypePlain );
	outputVarianceHSL->setDimensionLabels( "h", "s", "l" );
	outputVarianceHSL->setParent( hslGroup );
	outputVarianceHSL->setEvaluateOnChange( false );

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
	outputKurtosisHSL->setHint( hintKurtosis );
	outputKurtosisHSL->setDoubleType( OFX::eDoubleTypePlain );
	outputKurtosisHSL->setDimensionLabels( "h", "s", "l" );
	outputKurtosisHSL->setParent( hslGroup );
	outputKurtosisHSL->setEvaluateOnChange( false );

	OFX::Double3DParamDescriptor* outputSkewnessHSL = desc.defineDouble3DParam( kParamOutputSkewnessHSL );
	outputSkewnessHSL->setLabel( "Skewness" );
	outputSkewnessHSL->setHint( hintSkewness );
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
