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
void ResizePluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::EContext context )
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

	OFX::BooleanParamDescriptor* split = desc.defineBooleanParam( kParamSplit );
	split->setLabel( "Initial aspect ratio" );
	split->setDefault( true );
	split->setHint( "Keep initial aspect ratio." );

	OFX::ChoiceParamDescriptor* direction = desc.defineChoiceParam( kParamDirection );
	direction->setLabel( "Direction" );
	direction->appendOption( kParamSizeX );
	direction->appendOption( kParamSizeY );
	direction->setDefault( eParamSizeX );

	OFX::ChoiceParamDescriptor* type = desc.defineChoiceParam( kParamType );
	type->setLabel( "Type" );
	type->appendOption( kParamOutputFormat );
	type->appendOption( kParamScale );

	OFX::Double2DParamDescriptor* toBox = desc.defineDouble2DParam( kParamOutputFormat );
	toBox->setDefault( 200, 200 );
	toBox->setLabel( "Width / Height" );
	toBox->setHint( "Set the output RoD." );

	OFX::DoubleParamDescriptor* scaleRod = desc.defineDoubleParam( kParamScale );
	scaleRod->setLabel( "Scale" );
	scaleRod->setDefault( 1.0 );
	scaleRod->setRange( 0.1, std::numeric_limits<double>::max() );
	scaleRod->setDisplayRange( 0.1, 2.5 );
	scaleRod->setEnabled( false );
	scaleRod->setHint( "Adjust the output RoD." );

	OFX::DoubleParamDescriptor* scaleRodX = desc.defineDoubleParam( kParamScaleX );
	scaleRodX->setLabel( "Scale width" );
	scaleRodX->setDefault( 1.0 );
	scaleRodX->setRange( 0.1, std::numeric_limits<double>::max() );
	scaleRodX->setDisplayRange( 0.1, 2.5 );
	scaleRodX->setEnabled( false );
	scaleRodX->setHint( "Adjust width of the output RoD." );

	OFX::DoubleParamDescriptor* scaleRodY = desc.defineDoubleParam( kParamScaleY );
	scaleRodY->setLabel( "Scale height" );
	scaleRodY->setDefault( 1.0 );
	scaleRodY->setRange( 0.1, std::numeric_limits<double>::max() );
	scaleRodY->setDisplayRange( 0.1, 2.5 );
	scaleRodY->setEnabled( false );
	scaleRodY->setHint( "Adjust height of the output RoD." );

	// filters parameters //
	OFX::ChoiceParamDescriptor* filter = desc.defineChoiceParam( kParamFilter );
	filter->setLabel( "Filter" );
	filter->appendOption( kParamFilterNearest );
	filter->appendOption( kParamFilterBilinear );
	filter->appendOption( kParamFilterBicubic );
	filter->appendOption( kParamFilterKeys );
	filter->appendOption( kParamFilterSimon );
	filter->appendOption( kParamFilterRifman );
	filter->appendOption( kParamFilterLanczos );
	filter->setHint( "Select the filtering method." );
	filter->setDefault( eParamFilterBilinear );
//
//	OFX::Double2DParamDescriptor* scale = desc.defineDouble2DParam( kParamScale );
//	scale->setDefault( 1.0, 1.0 );
//	scale->setLabel( "Scale" );

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

