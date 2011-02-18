#include "LensDistortPluginFactory.hpp"
#include "LensDistortPlugin.hpp"
#include "LensDistortOverlayInteract.hpp"

#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {
namespace lens {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void LensDistortPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleLensDistort", "LensDistort",
	                "Create or correct lens distortion." );
	desc.setPluginGrouping( "tuttle/image/process/geometry" );

	desc.setDescription( "Apply or correct a lens distortion on an image." );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// set a few flags
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
	desc.setSupportsTiles( true );

	desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayWrap<LensDistortOverlayDescriptor>() );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void LensDistortPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc, OFX::EContext context )
{
	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( true );

	// create the mandated source clip
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( true );

	// declare an optional clip reference for RoD
	OFX::ClipDescriptor* srcRefClip = desc.defineClip( kClipOptionalSourceRef );
	srcRefClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcRefClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcRefClip->setSupportsTiles( true );
	srcRefClip->setOptional( true );
	srcRefClip->setLabel( "ref" );

	OFX::BooleanParamDescriptor* reverse = desc.defineBooleanParam( kParamReverse );
	reverse->setLabel( "Reverse" );
	reverse->setDefault( false );
	reverse->setHint( "Invert the effect.\n"
	                  "Distort becomes undistort, and vice versa." );

	// Controls
	OFX::BooleanParamDescriptor* displaySource = desc.defineBooleanParam( kParamDisplaySource );
	displaySource->setLabel( "displaySource" );
	displaySource->setDefault( false );
	displaySource->setHint( "Display the image source (usefull to parameter the distortion with lines overlays on the source image)." );

	OFX::ChoiceParamDescriptor* lensType = desc.defineChoiceParam( kParamLensType );
	lensType->setLabel( "Lens type" );
	lensType->appendOption( kParamLensTypeStandard );
	#ifndef TUTTLE_PRODUCTION
	lensType->appendOption( kParamLensTypeFishEye ); // not implemented yet...
	lensType->appendOption( kParamLensTypeAdvanced ); // not implemented yet...
	lensType->setIsSecret( true );
	#endif
	lensType->setDefault( 0 );

	OFX::DoubleParamDescriptor* coef1 = desc.defineDoubleParam( kParamCoef1 );
	coef1->setScriptName( "Main" );
	coef1->setDefault( 0.1 );
	coef1->setDisplayRange( -1.0, 1.0 );
	coef1->setHint( "Main distortion coeffecient\n"
	                ">0 : Barrel distortion\n"
	                "<0 : Pincushion distortion\n"
	                );

	OFX::DoubleParamDescriptor* coef2 = desc.defineDoubleParam( kParamCoef2 );
	coef2->setLabel( "Secondary" );
	coef2->setDefault( 0.0 );
	coef2->setDisplayRange( -1.0, 1.0 );
	coef2->setHint( "Secondary distortion coeffecient (usefull for fisheyes only)\n"
	                ">0 : Barrel distortion\n"
	                "<0 : Pincushion distortion\n"
	                );
	#ifdef TUTTLE_PRODUCTION
	coef2->setIsSecret( true );
	#endif

	OFX::DoubleParamDescriptor* squeeze = desc.defineDoubleParam( kParamSqueeze );
	squeeze->setLabel( "Squeeze" );
	#ifdef TUTTLE_PRODUCTION
	squeeze->setIsSecret( true );
	#endif
	//    squeeze->setDoubleType( eDoubleTypeNormalisedX );
	squeeze->setDefault( 1.0 );
	squeeze->setRange( 0.00001, 1.0 );
	squeeze->setDisplayRange( 0.01, 1.0 );
	squeeze->setHint( "Squeeze distortion coeffecient (usefull for bad quality lens...)" );

	OFX::Double2DParamDescriptor* asymmetric = desc.defineDouble2DParam( kParamAsymmetric );
	asymmetric->setLabel( "Asymmetric" );
	#ifdef TUTTLE_PRODUCTION
	asymmetric->setIsSecret( true );
	#endif
	//    asymmetric->setDoubleType( eDoubleTypeNormalisedXY );
	asymmetric->setDefault( 0.0, 0.0 );
	asymmetric->setRange( 0.0, 0.0, 1.0, 1.0 );
	asymmetric->setDisplayRange( 0.0, 0.0, 1.0, 1.0 );
	asymmetric->setHint( "asymmetric distortion coeffecient (usefull for bad quality lens...)" );

	OFX::Double2DParamDescriptor* center = desc.defineDouble2DParam( kParamCenter );
	center->setLabel( "Center" );
	center->setDoubleType( OFX::eDoubleTypePlain );
	center->setDefault( 0.0, 0.0 );
	center->setDisplayRange( -1.0, -1.0, 1.0, 1.0 );
	center->setHint( "Center parameter allows you to shift the center of distortion." );

	OFX::BooleanParamDescriptor* centerOverlay = desc.defineBooleanParam( kParamCenterOverlay );
	centerOverlay->setLabel( "Display distortion center" );
	centerOverlay->setDefault( false );
	centerOverlay->setEvaluateOnChange( false );
	centerOverlay->setHint( "Active distortion center point overlay." );

	OFX::ChoiceParamDescriptor* centerType = desc.defineChoiceParam( kParamCenterType );
	centerType->setLabel( "Center type" );
	#ifdef TUTTLE_PRODUCTION
	centerType->setIsSecret( true );
	#endif
	centerType->appendOption( kParamCenterTypeSource );
	centerType->appendOption( kParamCenterTypeRoW );
	centerType->setDefault( 0 );
	centerType->setHint( "Centered on source or output image." );

	OFX::DoubleParamDescriptor* preScale = desc.defineDoubleParam( kParamPreScale );
	preScale->setLabel( "Pre-scale" );
	//    preScale->setDoubleType( eDoubleTypeNormalisedXY );
	preScale->setDefault( 1.0 );
	preScale->setRange( 0.00001, std::numeric_limits<double>::max() );
	preScale->setDisplayRange( 0.0, 2.5 );
	preScale->setHint( "If the transformation of optics is high, you may need to change the scale of the result to be globally closer to the source image or preserve a good resolution." );

	OFX::DoubleParamDescriptor* postScale = desc.defineDoubleParam( kParamPostScale );
	postScale->setLabel( "Post-scale" );
	//    scale->setDoubleType( eDoubleTypeNormalisedXY );
	postScale->setDefault( 1.0 );
	postScale->setRange( 0.00001, std::numeric_limits<double>::max() );
	postScale->setDisplayRange( 0.0, 2.5 );
	postScale->setHint( "If the transformation of optics is high, you may need to change the scale of the result to be globally closer to the source image or preserve a good resolution." );

	OFX::ChoiceParamDescriptor* interpolation = desc.defineChoiceParam( kParamInterpolation );
	interpolation->setLabel( "Interpolation" );
	interpolation->appendOption( kParamInterpolationNearest );
	interpolation->appendOption( kParamInterpolationBilinear );
	interpolation->setDefault( 1 );
	interpolation->setHint( "Interpolation method" );

	OFX::ChoiceParamDescriptor* resizeRod = desc.defineChoiceParam( kParamResizeRod );
	resizeRod->setLabel( "Resize RoD" );
	resizeRod->appendOption( kParamResizeRodNo );
	resizeRod->appendOption( kParamResizeRodSourceRef );
	resizeRod->appendOption( kParamResizeRodMin );
	resizeRod->appendOption( kParamResizeRodMax );
	resizeRod->appendOption( kParamResizeRodManual );
	resizeRod->setDefault( 0 );
	resizeRod->setHint( "Resize output RoD" );

	OFX::DoubleParamDescriptor* scaleRod = desc.defineDoubleParam( kParamResizeRodManualScale );
	scaleRod->setLabel( "Scale RoD" );
	//    scaleRod->setDoubleType( eDoubleTypeNormalisedXY );
	//    scaleRod->setIsSecret( true );
	//    scaleRod->setEnabled( false );
	scaleRod->setDefault( 1.0 );
	scaleRod->setRange( 0.0, std::numeric_limits<double>::max() );
	scaleRod->setDisplayRange( 0, 2.5 );
	scaleRod->setHint( "Adjust the output RoD." );

	OFX::GroupParamDescriptor* displayOptions = desc.defineGroupParam( kParamDisplayOptions );
	displayOptions->setLabel( "Display options" );
	displayOptions->setHint( "Display options (change nothing on the image)" );

	OFX::BooleanParamDescriptor* displayGrid = desc.defineBooleanParam( kParamGridOverlay );
	displayGrid->setLabel( "Display grid" );
	displayGrid->setParent( *displayOptions );
	displayGrid->setDefault( false );
	displayGrid->setEvaluateOnChange( false );
	displayGrid->setHint( "Display the grid" );

	OFX::Double2DParamDescriptor* gridCenter = desc.defineDouble2DParam( kParamGridCenter );
	gridCenter->setLabel( "Grid center" );
	gridCenter->setDoubleType( OFX::eDoubleTypePlain );
	gridCenter->setParent( *displayOptions );
	gridCenter->setDefault( 0.0, 0.0 );
	gridCenter->setDisplayRange( -1.0, -1.0, 1.0, 1.0 );
	gridCenter->setEvaluateOnChange( false );
	gridCenter->setHint( "Allows you to shift the center of the  display grid." );

	OFX::BooleanParamDescriptor* gridCenterOverlay = desc.defineBooleanParam( kParamGridCenterOverlay );
	gridCenterOverlay->setLabel( "Display grid center" );
	gridCenterOverlay->setParent( *displayOptions );
	gridCenterOverlay->setDefault( false );
	gridCenterOverlay->setEvaluateOnChange( false );
	gridCenterOverlay->setHint( "Active grid center point overlay." );

	OFX::Double2DParamDescriptor* gridScale = desc.defineDouble2DParam( kParamGridScale );
	gridScale->setLabel( "Grid scale" );
	gridScale->setDoubleType( OFX::eDoubleTypePlain );
	gridScale->setParent( *displayOptions );
	gridScale->setDefault( 1.0, 1.0 );
	gridScale->setDisplayRange( -10.0, -10.0, 10.0, 10.0 );
	gridScale->setEvaluateOnChange( false );
	gridScale->setHint( "Allows you to scale the display grid." );

	OFX::GroupParamDescriptor* debugOptions = desc.defineGroupParam( kParamDebugOptions );
	debugOptions->setLabel( "Debug options" );
	debugOptions->setHint( "Debug options" );
	debugOptions->setParent( *displayOptions );
	#ifdef TUTTLE_PRODUCTION
	debugOptions->setIsSecret( true );
	#endif

	OFX::BooleanParamDescriptor* debugDisplayRoi = desc.defineBooleanParam( kParamDebugDisplayRoi );
	debugDisplayRoi->setLabel( "Display RoI" );
	#ifdef TUTTLE_PRODUCTION
	debugDisplayRoi->setIsSecret( true );
	#endif
	debugDisplayRoi->setParent( *debugOptions );
	debugDisplayRoi->setDefault( false );
	debugDisplayRoi->setEvaluateOnChange( false );
	debugDisplayRoi->setHint( "Display RoI" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in]   handle  effect handle
 * @param[in]   context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* LensDistortPluginFactory::createInstance( OfxImageEffectHandle handle, OFX::EContext context )
{
	return new LensDistortPlugin( handle );
}

}
}
}
