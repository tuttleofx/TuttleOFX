#include "DiffPlotPluginFactory.hpp"
#include "DiffPlotPlugin.hpp"
#include "DiffPlotDefinitions.hpp"
#include "DiffPlotOverlay.hpp"

#include <limits>

namespace tuttle {
namespace plugin {
namespace diffPlot {

static const bool kSupportTiles = true;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void DiffPlotPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	// describe the plugin
	desc.setLabels( "TuttleDiffPlot", "DiffPlot",
		            "DiffPlot" );
	desc.setPluginGrouping( "tuttle/image/display" );

	desc.setDescription(
		"DiffPlot\n"
		"\n"
		"Display channel values correspondences between 2 images.\n"
		"It will look like an 1D LUT, but for one value in the first image, "
		"multiple values could exist in the 2nd image.\n"
	);

	// add the supported contexts, only filter at the moment
//	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );

	desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayWrap<DiffPlotOverlayDescriptor>() );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void DiffPlotPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
//	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
//	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );
	
	OFX::ClipDescriptor* srcBClip = desc.defineClip( kClipSourceB );
	srcBClip->addSupportedComponent( OFX::ePixelComponentRGBA );
//	srcBClip->addSupportedComponent( OFX::ePixelComponentRGB );
//	srcBClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcBClip->setSupportsTiles( kSupportTiles );

	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
//	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
//	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	//global display
	OFX::BooleanParamDescriptor* boolGLOBAL = desc.defineBooleanParam(kGlobalDisplay);
	boolGLOBAL->setHint("Display global overlay on screen.");
	boolGLOBAL->setDefault(true);
	
	//process mode
	OFX::ChoiceParamDescriptor* colorMapping = desc.defineChoiceParam(kParamColorMapping);
	colorMapping->setLabel("Color Mapping");
	colorMapping->setHint("Process channel by channel independently or compare full colors triplets");
	colorMapping->appendOption("per channel");
	colorMapping->appendOption("color triplet");

    // RGB / HSL
	{
		//Group Param (RGB & HSL)
		OFX::GroupParamDescriptor *groupRGB = desc.defineGroupParam(kGroupRGB);
		groupRGB->setLabel(kGroupRGBLabel);
		OFX::GroupParamDescriptor *groupHSL = desc.defineGroupParam(kGroupHSL);
		groupHSL->setLabel(kGroupHSLLabel);

		//Channels checkboxes (RGB)
		OFX::BooleanParamDescriptor* boolR = desc.defineBooleanParam(kBoolRed);
		boolR->setDefault(true);							//red channel is not selected by default
		boolR->setHint("Activate Red channel");
		boolR->setLayoutHint( OFX::eLayoutHintNoNewLine ); //line is not finished
		boolR->setParent(groupRGB);
		//red multiplier
		OFX::DoubleParamDescriptor* redMultiplier = desc.defineDoubleParam(kMultiplierRed);
		redMultiplier->setLabel(kMultiplierLabel);
		redMultiplier->setHint("Determinate curve from selection precision.");
		redMultiplier->setRange(1, 1000);
		redMultiplier->setDisplayRange(0,5);
		redMultiplier->setDefault(1);
		redMultiplier->setParent(groupRGB);
		
		OFX::BooleanParamDescriptor* boolG = desc.defineBooleanParam(kBoolGreen);
		boolG->setDefault(true);						//green channel is not selected by default
		boolG->setHint("Activate Green channel");
		boolG->setLayoutHint( OFX::eLayoutHintNoNewLine ); //line is not finished
		boolG->setParent(groupRGB);
		//green multiplier
		OFX::DoubleParamDescriptor* greenMultiplier = desc.defineDoubleParam(kMultiplierGreen);
		greenMultiplier->setLabel(kMultiplierLabel);
		greenMultiplier->setHint("Determinate curve from selection precision.");
		greenMultiplier->setRange(1, 1000);
		greenMultiplier->setDisplayRange(0,5);
		greenMultiplier->setDefault(1);
		greenMultiplier->setParent(groupRGB);
		
		OFX::BooleanParamDescriptor* boolB = desc.defineBooleanParam(kBoolBlue);
		boolB->setHint("Activate Blue channel");
		boolB->setLayoutHint( OFX::eLayoutHintNoNewLine ); //line is not finished
		boolB->setDefault(true);						   //blue channel is not selected by default
		boolB->setParent(groupRGB);
		//blue multiplier
		OFX::DoubleParamDescriptor* blueMultiplier = desc.defineDoubleParam(kMultiplierBlue);
		blueMultiplier->setLabel(kMultiplierLabel);
		blueMultiplier->setHint("Determinate curve from selection precision.");
		blueMultiplier->setRange(1, 1000);
		blueMultiplier->setDisplayRange(0,5);
		blueMultiplier->setDefault(1);
		blueMultiplier->setParent(groupRGB);
		
		//Channels check box (HSL)
		OFX::BooleanParamDescriptor* boolH = desc.defineBooleanParam(kBoolHue);
		boolH->setDefault(true);
		boolH->setHint("Activate Hue channel");
		boolH->setLayoutHint( OFX::eLayoutHintNoNewLine ); //line is not finished
		boolH->setParent(groupHSL);
		//Hue multiplier
		OFX::DoubleParamDescriptor* hueMultiplier = desc.defineDoubleParam(kMultiplierHue);
		hueMultiplier->setLabel(kMultiplierLabel);
		hueMultiplier->setHint("Determinate curve from selection precision.");
		hueMultiplier->setRange(1, 1000);
		hueMultiplier->setDisplayRange(0,5);
		hueMultiplier->setDefault(1);
		hueMultiplier->setParent(groupHSL);
		
		OFX::BooleanParamDescriptor* boolS = desc.defineBooleanParam(kBoolSaturation);
		boolS->setDefault(true);
		boolS->setHint("Activate Saturation channel");
		boolS->setLayoutHint( OFX::eLayoutHintNoNewLine ); //line is not finished
		boolS->setParent(groupHSL);
		//Saturation multiplier
		OFX::DoubleParamDescriptor* saturationMultiplier = desc.defineDoubleParam(kMultiplierSaturation);
		saturationMultiplier->setLabel(kMultiplierLabel);
		saturationMultiplier->setHint("Determinate curve from selection precision.");
		saturationMultiplier->setRange(1, 1000);
		saturationMultiplier->setDisplayRange(0,5);
		saturationMultiplier->setDefault(1);
		saturationMultiplier->setParent(groupHSL);
		
		OFX::BooleanParamDescriptor* boolL = desc.defineBooleanParam(kBoolLightness);
		boolL->setHint("Activate Lightness channel");
		boolL->setLayoutHint( OFX::eLayoutHintNoNewLine ); //line is not finished
		boolL->setDefault(true);
		boolL->setParent(groupHSL);
		//Lightness multiplier
		OFX::DoubleParamDescriptor* lightnessMultiplier = desc.defineDoubleParam(kMultiplierLightness);
		lightnessMultiplier->setLabel(kMultiplierLabel);
		lightnessMultiplier->setHint("Determinate curve from selection precision.");
		lightnessMultiplier->setRange(1, 1000);
		lightnessMultiplier->setDisplayRange(0,5);
		lightnessMultiplier->setDefault(1);
		lightnessMultiplier->setParent(groupHSL);
		
		//Close RGB group (group states by default on screen)
		groupRGB->setOpen(true);
		groupHSL->setOpen(true);
	}
	
	//DiffPlot overlay group
	{
		OFX::GroupParamDescriptor *groupDiffPlotOverlay = desc.defineGroupParam(kGroupDiffPlotOverlay);
		groupDiffPlotOverlay->setLabel(kGroupDiffPlotOverlayLabel);
		groupDiffPlotOverlay->setOpen(true);
//		groupDiffPlotOverlay->setAsTab();

		//DiffPlot display settings
		OFX::ChoiceParamDescriptor* gammaType = desc.defineChoiceParam(kHistoDisplayListParamLabel);
		gammaType->setLabel(kHistoDisplayListParamLabel);
		gammaType->setEvaluateOnChange(false); // don't need to recompute on change
		gammaType->setHint("DiffPlot display \n -global : normalize all of channels \n -by channel : keep proportions between channels");
		gammaType->appendOption(kHistoDisplayListParamOpt2);
		gammaType->appendOption(kHistoDisplayListParamOpt1);
		gammaType->setParent(groupDiffPlotOverlay);	

		//nbOfstep (advanced group)
		OFX::IntParamDescriptor* nbStepRange = desc.defineIntParam(knbStepRange);
		nbStepRange->setLabel(knbStepRangeLabel);
		nbStepRange->setHint("Determinate diffPlot overlay precision.");
		nbStepRange->setRange(1, 1000);
		nbStepRange->setDisplayRange(1, 600.0 );
		nbStepRange->setDefault(255);
		nbStepRange->setEvaluateOnChange(false); // don't need to recompute on change
		nbStepRange->setParent(groupDiffPlotOverlay);

		//selection multiplier (advanced group)
		OFX::DoubleParamDescriptor* selectionMultiplier = desc.defineDoubleParam(kselectionMultiplier);
		selectionMultiplier->setLabel(kselectionMultiplierLabel);
		selectionMultiplier->setHint("With high values, small selection are more visible.");
		selectionMultiplier->setRange(0.001,1000.0);
		selectionMultiplier->setDisplayRange(0.0, 100.0 );
		selectionMultiplier->setDefault(2.0);
		selectionMultiplier->setEvaluateOnChange(false); // don't need to recompute on change
		selectionMultiplier->setParent(groupDiffPlotOverlay);

		//Refresh diffPlots overlay Button
		OFX::PushButtonParamDescriptor* refreshOverlayButton = desc.definePushButtonParam(kButtonRefreshOverlay);
		refreshOverlayButton->setLabel(kButtonRefreshOverlayLabel);
		refreshOverlayButton->setHint("Refresh diffPlot overlay.");
		refreshOverlayButton->setParent(groupDiffPlotOverlay);
	}
	
	//Selection group
	{
		OFX::GroupParamDescriptor *groupSelection = desc.defineGroupParam(kGroupSelection);
		groupSelection->setLabel(kGroupSelectionLabel);
		groupSelection->setOpen(false);
//		groupSelection->setAsTab();
		//display selection
		OFX::BooleanParamDescriptor* boolDisplaySelection = desc.defineBooleanParam(kBoolSelection);
		boolDisplaySelection->setDefault(true);
		boolDisplaySelection->setEvaluateOnChange(false);// don't need to recompute on change
		boolDisplaySelection->setHint("Display the selected zone on screen.");
		boolDisplaySelection->setParent(groupSelection);
		//clear selection
		OFX::PushButtonParamDescriptor* resetSelectionButton = desc.definePushButtonParam(kButtonResetSelection);
		resetSelectionButton->setLabel(kButtonResetSelectionLabel);
		resetSelectionButton->setHint("Reset user's selection.");
		resetSelectionButton->setParent(groupSelection);
		//selection mode
		OFX::ChoiceParamDescriptor* selectionMode = desc.defineChoiceParam(kSelectionModeListParamLabel);
		selectionMode->setLabel(kSelectionModeListParamLabel);
		selectionMode->setHint( "Selection mode \n - unique : reset past selection before selection \n - additive : add pixels to current selection \n -subtractive : remote pixel from current selection");
		selectionMode->appendOption(kSelectionModeListParamOpt2);
		selectionMode->appendOption(kSelectionModeListParamOpt1);
		selectionMode->appendOption(kSelectionModeListParamOpt3);
		selectionMode->setParent(groupSelection);
	}
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* DiffPlotPluginFactory::createInstance( OfxImageEffectHandle handle,OFX::EContext context )
{
	return new DiffPlotPlugin( handle );
}

}
}
}

