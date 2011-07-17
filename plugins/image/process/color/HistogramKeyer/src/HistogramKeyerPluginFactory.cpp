#include "HistogramKeyerPluginFactory.hpp"
#include "HistogramKeyerPlugin.hpp"
#include "HistogramKeyerDefinitions.hpp"
#include "HistogramKeyerOverlay.hpp"

#include <limits>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void HistogramKeyerPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	// describe the plugin
	desc.setLabels( "TuttleHistogramKeyer", "HistogramKeyer",
		            "HistogramKeyer" );
	desc.setPluginGrouping( "tuttle/image/process/color" );

	desc.setDescription(
		"HistogramKeyer\n"
		"This histogram keyer plugin allows user to create an alpha mask using HSL & RGB curves. Output can be in gray scale or directly in alpha channel (RGBA)."
		"There are some selection parameters which could help you to refine your maniplulation (control points unders histograms and quantity)."
		"A reverse output mask is also implemented.\n"
		"\n"
	);

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

	desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayWrap<HistogramKeyerOverlayDescriptor>() );

	if( ! OFX::getImageEffectHostDescription()->supportsParametricParameter )
	{
		BOOST_THROW_EXCEPTION( exception::MissingHostFeature( "Parametric parameter" ) );
	}
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void HistogramKeyerPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,OFX::EContext context )
{
	
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	//global display
	OFX::BooleanParamDescriptor* boolGLOBAL = desc.defineBooleanParam(kGlobalDisplay);
	boolGLOBAL->setDefault(true);
		
    // if parametric parameters are supported
	if( OFX::getImageEffectHostDescription()->supportsParametricParameter )
	{
		OFX::ParametricParamDescriptor* curvesRGB = desc.defineParametricParam( kParamRGBColorSelection );
		OFX::ParametricParamDescriptor* curvesHSL = desc.defineParametricParam( kParamHSLColorSelection );
		
		//Group Param (RGB & HSL)
		OFX::GroupParamDescriptor *groupRGB = desc.defineGroupParam(kGroupRGB);
		groupRGB->setLabel(kGroupRGBLabel);
		OFX::GroupParamDescriptor *groupHSL = desc.defineGroupParam(kGroupHSL);
		groupHSL->setLabel(kGroupHSLLabel);

		//define the graphic aspect
		curvesRGB->setRange( 0.0, 1.0 );
		curvesHSL->setRange( 0.0, 1.0 );
		curvesRGB->setDimension(nbCurvesRGB);
		curvesHSL->setDimension(nbCurvesHSL);

		//Add curves RGB
		curvesRGB->setDimensionLabel( kParamColorSelectionRed, 0 );
		curvesRGB->setDimensionLabel( kParamColorSelectionGreen, 1 );
		curvesRGB->setDimensionLabel( kParamColorSelectionBlue, 2 );
		//Add curves HSL
		curvesHSL->setDimensionLabel( kParamColorSelectionHue, 0 );
		curvesHSL->setDimensionLabel( kParamColorSelectionSaturation, 1 );
		curvesHSL->setDimensionLabel( kParamColorSelectionLightness, 2 );
		//define curves color RGB 
		curvesRGB->setHint( "Color selection" );
		static const OfxRGBColourD red   = {1,0,0};
		static const OfxRGBColourD green = {0,1,0};
		static const OfxRGBColourD blue  = {0,0,1};
		curvesRGB->setUIColour( 0, red );
		curvesRGB->setUIColour( 1, green );
		curvesRGB->setUIColour( 2, blue );
		//define curves color HSL 
		curvesHSL->setHint( "Color selection" );
		curvesHSL->setUIColour( 0, red );
		curvesHSL->setUIColour( 1, green );
		curvesHSL->setUIColour( 2, blue );

		curvesRGB->setInteractDescriptor( new OFX::DefaultParamInteractWrap<RGBParamOverlayDescriptor>() );	//attach parametric curve to RGBOverlay
		curvesHSL->setInteractDescriptor( new OFX::DefaultParamInteractWrap<HSLParamOverlayDescriptor>() );	//attach parametric curve to HSLOverlay
		
		//add curves to their groups
		curvesRGB->setParent(groupRGB);
		curvesHSL->setParent(groupHSL);
		
		//Set each curves to initial value
		curvesRGB->setIdentity();
		curvesHSL->setIdentity();
		//add 2 control points (0,1) and (1,1) for each channel
		for(unsigned int i=0; i< nbCurvesRGB; ++i)
		{
			//curvesRGB->addControlPoint( i, 0.0, 0.0, 1.0, false );
			curvesRGB->addControlPoint( i, 0.0, 1.0, 1.0, false );
		}
		for(unsigned int i=0; i< nbCurvesHSL; ++i)
		{
			//curvesHSL->addControlPoint( i, 0.0, 0.0, 1.0, false );
			curvesHSL->addControlPoint( i, 0.0, 1.0, 1.0, false );
		}
		
		//Channels checkboxes (RGB)
		OFX::BooleanParamDescriptor* boolR = desc.defineBooleanParam(kBoolRed);
		boolR->setDefault(false);
		boolR->setParent(groupRGB);
		OFX::BooleanParamDescriptor* boolG = desc.defineBooleanParam(kBoolGreen);
		boolG->setDefault(false);
		boolG->setParent(groupRGB);
		OFX::BooleanParamDescriptor* boolB = desc.defineBooleanParam(kBoolBlue);
		boolB->setDefault(false);
		boolB->setParent(groupRGB);
		
		//Channels check box (HSL)
		OFX::BooleanParamDescriptor* boolH = desc.defineBooleanParam(kBoolHue);
		boolH->setDefault(false);
		boolH->setParent(groupHSL);
		OFX::BooleanParamDescriptor* boolS = desc.defineBooleanParam(kBoolSaturation);
		boolS->setDefault(false);
		boolS->setParent(groupHSL);
		OFX::BooleanParamDescriptor* boolL = desc.defineBooleanParam(kBoolLightness);
		boolL->setDefault(false);
		boolL->setParent(groupHSL);
		
		//Clean Button (RGB / HSL)
		OFX::PushButtonParamDescriptor* cleanButtonRGB = desc.definePushButtonParam(kButtonCleanRGB);
		cleanButtonRGB->setLabel(kButtonCleanRGBLabel);
		cleanButtonRGB->setParent(groupRGB);
		
		OFX::PushButtonParamDescriptor* cleanButtonHSL = desc.definePushButtonParam(kButtonCleanHSL);
		cleanButtonHSL->setLabel(kButtonCleanHSLLabel);
		cleanButtonHSL->setParent(groupHSL);
		
		//Close groups (closed by default on screen)
		groupRGB->setOpen(false);
		groupHSL->setOpen(true);
	}
	
	//Histogram overlay group
	{
		OFX::GroupParamDescriptor *groupHistogramOverlay = desc.defineGroupParam(kGroupHistogramOverlay);
		groupHistogramOverlay->setLabel(kGroupHistogramOverlayLabel);
		groupHistogramOverlay->setOpen(true);
		groupHistogramOverlay->setAsTab();

		//Histogram display settings
		OFX::ChoiceParamDescriptor* gammaType = desc.defineChoiceParam(kHistoDisplayListParamLabel);
		gammaType->setLabel(kHistoDisplayListParamLabel);
		gammaType->setEvaluateOnChange(false); // don't need to recompute on change
		gammaType->setHint( "Histogram display list");
		gammaType->appendOption(kHistoDisplayListParamOpt2);
		gammaType->appendOption(kHistoDisplayListParamOpt1);
		gammaType->setParent(groupHistogramOverlay);	

		//Clean all Button
		OFX::PushButtonParamDescriptor* cleanButtonAll = desc.definePushButtonParam(kButtonCleanAll);
		cleanButtonAll->setLabel(kButtonCleanAllLabel);
		cleanButtonAll->setParent(groupHistogramOverlay);
	}
	
	//Selection group
	{
		OFX::GroupParamDescriptor *groupSelection = desc.defineGroupParam(kGroupSelection);
		groupSelection->setLabel(kGroupSelectionLabel);
		groupSelection->setOpen(false);
		groupSelection->setAsTab();
		//display selection
		OFX::BooleanParamDescriptor* boolDisplaySelection = desc.defineBooleanParam(kBoolSelection);
		boolDisplaySelection->setDefault(true);
		boolDisplaySelection->setEvaluateOnChange(false);// don't need to recompute on change
		boolDisplaySelection->setParent(groupSelection);
		//clear selection
		OFX::PushButtonParamDescriptor* clearSelectionButton = desc.definePushButtonParam(kButtonClearSelection);
		clearSelectionButton->setLabel(kButtonClearSelectionLabel);
		clearSelectionButton->setParent(groupSelection);
	}
	
	///Advanced group
	{
		OFX::GroupParamDescriptor *groupAdvanced = desc.defineGroupParam(kGroupAdvanced);
		groupAdvanced->setLabel(kGroupAdvancedLabel);
		groupAdvanced->setOpen(false);
		groupAdvanced->setAsTab();
		
		//nbOfstep (advanced group)
		OFX::IntParamDescriptor* nbStepRange = desc.defineIntParam(knbStepRange);
		nbStepRange->setLabel(knbStepRangeLabel);
		nbStepRange->setRange(1, 1000);
		nbStepRange->setDisplayRange(1, 600.0 );
		nbStepRange->setDefault(255);
		nbStepRange->setEvaluateOnChange(false); // don't need to recompute on change
		nbStepRange->setParent(groupAdvanced);
		//selection multiplier (advanced group)
		OFX::DoubleParamDescriptor* selectionMultiplier = desc.defineDoubleParam(kselectionMultiplier);
		selectionMultiplier->setLabel(kselectionMultiplierLabel);
		selectionMultiplier->setRange(0.001,1000.0);
		selectionMultiplier->setDisplayRange(0.0, 100.0 );
		selectionMultiplier->setDefault(2.0);
		selectionMultiplier->setEvaluateOnChange(false); // don't need to recompute on change
		selectionMultiplier->setParent(groupAdvanced);

		//Refresh histograms overlay Button
		OFX::PushButtonParamDescriptor* refreshOverlayButton = desc.definePushButtonParam(kButtonRefreshOverlay);
		refreshOverlayButton->setLabel(kButtonRefreshOverlayLabel);
		refreshOverlayButton->setParent(groupAdvanced);
	}
	//Output settings
	OFX::ChoiceParamDescriptor* outputType = desc.defineChoiceParam(kOutputListParamLabel);
	outputType->setLabel(kOutputListParamLabel);
	outputType->setHint( "Output settings list");
	outputType->appendOption(kOutputListParamOpt1);
	outputType->appendOption(kOutputListParamOpt2);
	outputType->setLayoutHint( OFX::eLayoutHintNoNewLine );

	//Reverse mask
	OFX::BooleanParamDescriptor* boolReverseMask = desc.defineBooleanParam(kBoolReverseMask);
	boolReverseMask->setDefault(false);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* HistogramKeyerPluginFactory::createInstance( OfxImageEffectHandle handle,OFX::EContext context )
{
	return new HistogramKeyerPlugin( handle );
}

}
}
}

