#include "ResizePluginFactory.hpp"
#include "ResizePlugin.hpp"
#include "ResizeDefinitions.hpp"

#include <limits>

namespace tuttle {
namespace plugin {
namespace resize {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ResizePluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleResize", "Resize",
			"Resize" );
	desc.setPluginGrouping( "tuttle/image/process/geometry" );

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
void ResizePluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc, OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent	( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent	( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent	( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles	( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent	( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent	( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent	( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles	( kSupportTiles );

	OFX::ChoiceParamDescriptor* options = desc.defineChoiceParam( kParamOptions );
	options->setLabel	( "type" );
	options->appendOption	( kParamFormat );
	options->appendOption	( kParamBox );
	options->appendOption	( kParamScale );
	options->setDefault	( eParamFormat );

	OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamFormat );
	format->setLabel	( "output format" );
	format->appendOption	( kParamPCVideo );
	format->appendOption	( kParamNTSC );
	format->appendOption	( kParamPAL );
	format->appendOption	( kParamHD );
	format->appendOption	( kParamNTSC169 );
	format->appendOption	( kParamPAL169 );
	format->appendOption	( kParam1kSuper35 );
	format->appendOption	( kParam1kCinemascope );
	format->appendOption	( kParam2kSuper35 );
	format->appendOption	( kParam2kCinemascope );
	format->appendOption	( kParam4kSuper35 );
	format->appendOption	( kParam4kCinemascope );
	format->appendOption	( kParamSquare256 );
	format->appendOption	( kParamSquare512 );
	format->appendOption	( kParamSquare1k );
	format->appendOption	( kParamSquare2k );
	format->setDefault	( eParam2kCinemascope );

	OFX::BooleanParamDescriptor* split = desc.defineBooleanParam( kParamSplit );
	split->setLabel			( "Initial aspect ratio" );
	split->setDefault		( true );
	split->setHint			( "Keep initial aspect ratio." );

	OFX::ChoiceParamDescriptor* direction = desc.defineChoiceParam( kParamDirection );
	direction->setLabel		( "Direction" );
	direction->appendOption		( kParamSizeX );
	direction->appendOption		( kParamSizeY );
	direction->setDefault		( eParamSizeX );

	OFX::DoubleParamDescriptor* scaleRod = desc.defineDoubleParam( kParamScale );
	scaleRod->setLabel		( "Scale" );
	scaleRod->setDefault		( 1.0 );
	scaleRod->setRange		( 0.1, std::numeric_limits<double>::max() );
	scaleRod->setDisplayRange	( 0.1, 2.5 );
	scaleRod->setHint		( "Adjust the output RoD." );

	OFX::DoubleParamDescriptor* toBoxDim = desc.defineDoubleParam( kParamOutputDimension );
	toBoxDim->setDefault		( 200 );
	toBoxDim->setRange		( 0, std::numeric_limits<double>::max() );
	toBoxDim->setDisplayRange	( 0, 3000 );
	toBoxDim->setLabel		( "Size" );
	toBoxDim->setHint		( "Set the output size (pixels)." );

	OFX::Double2DParamDescriptor* toBox = desc.defineDouble2DParam( kParamOutputFormat );
	toBox->setDefault		( 200, 200 );
	toBox->setLabel			( "Width / Height" );
	toBox->setHint			( "Set the output RoD." );

	OFX::DoubleParamDescriptor* scaleRodX = desc.defineDoubleParam( kParamScaleX );
	scaleRodX->setLabel		( "Width scale" );
	scaleRodX->setDefault		( 1.0 );
	scaleRodX->setRange		( 0.1, std::numeric_limits<double>::max() );
	scaleRodX->setDisplayRange	( 0.1, 2.5 );
	scaleRodX->setHint		( "Adjust width of the output RoD." );

	OFX::DoubleParamDescriptor* scaleRodY = desc.defineDoubleParam( kParamScaleY );
	scaleRodY->setLabel		( "Height scale" );
	scaleRodY->setDefault		( 1.0 );
	scaleRodY->setRange		( 0.1, std::numeric_limits<double>::max() );
	scaleRodY->setDisplayRange	( 0.1, 2.5 );
	scaleRodY->setHint		( "Adjust height of the output RoD." );

	OFX::BooleanParamDescriptor* center = desc.defineBooleanParam( kParamCenter );
	center->setLabel		( "Center resizing" );
	center->setDefault		( true );
	center->setHint			( "Set the resize to the center point." );

	OFX::Double2DParamDescriptor* centerPoint = desc.defineDouble2DParam( kParamCenterPoint );
	centerPoint ->setDefault	( 100, 100 );
	centerPoint ->setLabel		( "Center point at" );
	centerPoint ->setHint		( "Position of the center point." );

	// filters parameters //
	OFX::ChoiceParamDescriptor* filter = desc.defineChoiceParam( kParamFilter );
	filter->setLabel	( "Filter" );
	filter->appendOption	( kParamFilterNearest );
	filter->appendOption	( kParamFilterBilinear );
	filter->appendOption	( kParamFilterBC );
	filter->appendOption	( kParamFilterBicubic );
	filter->appendOption	( kParamFilterCatrom ); // idem as the Keys resampling
	filter->appendOption	( kParamFilterKeys );
	filter->appendOption	( kParamFilterSimon );
	filter->appendOption	( kParamFilterRifman );
	filter->appendOption	( kParamFilterMitchell );
	filter->appendOption	( kParamFilterParzen );
	filter->appendOption	( kParamFilterLanczos );
	filter->appendOption	( kParamFilterLanczos3 );
	filter->appendOption	( kParamFilterLanczos4 );
	filter->appendOption	( kParamFilterLanczos6 );
	filter->appendOption	( kParamFilterLanczos12 );
	filter->appendOption	( kParamFilterGaussian );
	filter->setHint		( "Select the filtering method." );
	filter->setDefault	( eParamFilterBilinear );

	OFX::DoubleParamDescriptor* B = desc.defineDoubleParam( kParamFilterB );
	B->setLabel			( "B value" );
	B->setDefault			( 1.0 );
	B->setRange			( 0.0, 1.0 );
	B->setDisplayRange		( 0.0, 1.0 );
	B->setHint			( "Adjust the B valuye of the cubic filter." );

	OFX::DoubleParamDescriptor* C = desc.defineDoubleParam( kParamFilterC );
	C->setLabel			( "C value" );
	C->setDefault			( 0.0 );
	C->setRange			( 0.0, 1.0 );
	C->setDisplayRange		( 0.0, 1.0 );
	C->setHint			( "Adjust the C valuye of the cubic filter." );

	OFX::DoubleParamDescriptor* filterSize = desc.defineDoubleParam( kParamFilterSize );
	filterSize->setLabel		( "Filter size" );
	filterSize->setDefault		( 3.0 );
	filterSize->setRange		( 1.0, 30.0 );
	filterSize->setDisplayRange	( 1.0, 30.0 );
	filterSize->setHint		( "Set the filter size." );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ResizePluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new ResizePlugin( handle );
}

}
}
}

