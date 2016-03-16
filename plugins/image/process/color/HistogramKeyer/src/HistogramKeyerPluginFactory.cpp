#include "HistogramKeyerPluginFactory.hpp"
#include "HistogramKeyerPlugin.hpp"
#include "HistogramKeyerDefinitions.hpp"
#include "HistogramKeyerOverlay.hpp"

#include <limits>

namespace tuttle
{
namespace plugin
{
namespace histogramKeyer
{

static const bool kSupportTiles = true;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void HistogramKeyerPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    // describe the plugin
    desc.setLabels("TuttleHistogramKeyer", "HistogramKeyer", "HistogramKeyer");
    desc.setPluginGrouping("tuttle/image/process/color");

    desc.setDescription("HistogramKeyer\n"
                        "This histogram keyer plugin allows user to create an alpha mask using HSL & RGB curves. Output can "
                        "be in gray scale or directly in alpha channel (RGBA)."
                        "There are some selection parameters which could help you to refine your maniplulation (control "
                        "points unders histograms and quantity)."
                        "A reverse output mask is also implemented.\n"
                        "\n");

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(OFX::eContextFilter);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // plugin flags
    desc.setSupportsTiles(kSupportTiles);
    desc.setRenderThreadSafety(OFX::eRenderFullySafe);

    desc.setOverlayInteractDescriptor(new OFX::DefaultEffectOverlayWrap<HistogramKeyerOverlayDescriptor>());

    if(!OFX::getImageEffectHostDescription()->supportsParametricParameter)
    {
        BOOST_THROW_EXCEPTION(exception::MissingHostFeature("Parametric parameter"));
    }
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void HistogramKeyerPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{

    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClip->setSupportsTiles(kSupportTiles);

    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    // global display
    OFX::BooleanParamDescriptor* boolGLOBAL = desc.defineBooleanParam(kGlobalDisplay);
    boolGLOBAL->setHint("Display global overlay on screen.");
    boolGLOBAL->setDefault(true);

    // if parametric parameters are supported
    if(OFX::getImageEffectHostDescription()->supportsParametricParameter)
    {
        OFX::ParametricParamDescriptor* curvesRGB = desc.defineParametricParam(kParamRGBColorSelection);
        OFX::ParametricParamDescriptor* curvesHSL = desc.defineParametricParam(kParamHSLColorSelection);

        // Group Param (RGB & HSL)
        OFX::GroupParamDescriptor* groupRGB = desc.defineGroupParam(kGroupRGB);
        groupRGB->setLabel(kGroupRGBLabel);
        OFX::GroupParamDescriptor* groupHSL = desc.defineGroupParam(kGroupHSL);
        groupHSL->setLabel(kGroupHSLLabel);

        // define the graphic aspect
        curvesRGB->setRange(0.0, 1.0); // set range on RGB curve
        curvesHSL->setRange(0.0, 1.0); // set range on HSL curve
        curvesRGB->setDimension(nbCurvesRGB); // 3 curves on RGB
        curvesHSL->setDimension(nbCurvesHSL); // 3 curves on HSL

        // Add curves RGB
        curvesRGB->setDimensionLabel(kParamColorSelectionRed, 0);   // 0 on RGB is red
        curvesRGB->setDimensionLabel(kParamColorSelectionGreen, 1); // 1 on RGB is green
        curvesRGB->setDimensionLabel(kParamColorSelectionBlue, 2);  // 2 on RGB is blue
        // Add curves HSL
        curvesHSL->setDimensionLabel(kParamColorSelectionHue, 0);        // 0 on HSL is hue
        curvesHSL->setDimensionLabel(kParamColorSelectionSaturation, 1); // 1 on HSL is saturation
        curvesHSL->setDimensionLabel(kParamColorSelectionLightness, 2);  // 2 on HSK is lightness
        // define curves color RGB
        curvesRGB->setHint("Color selection");
        static const OfxRGBColourD red = {1, 0, 0}; // set red color to red curve
        static const OfxRGBColourD green = {0, 1, 0}; // set green color to green curve
        static const OfxRGBColourD blue = {0, 0, 1}; // set blue color to blue curve
        curvesRGB->setUIColour(0, red);
        curvesRGB->setUIColour(1, green);
        curvesRGB->setUIColour(2, blue);
        // define curves color HSL
        curvesHSL->setHint("Color selection");
        curvesHSL->setUIColour(0, red); // set red color on hue curve
        curvesHSL->setUIColour(1, green); // set green color on saturation curve
        curvesHSL->setUIColour(2, blue); // set lightness color on saturation curve

        curvesRGB->setInteractDescriptor(
            new OFX::DefaultParamInteractWrap<RGBParamOverlayDescriptor>()); // attach parametric curve to RGBOverlay
        curvesHSL->setInteractDescriptor(
            new OFX::DefaultParamInteractWrap<HSLParamOverlayDescriptor>()); // attach parametric curve to HSLOverlay

        // add curves to their groups
        curvesRGB->setParent(groupRGB); // add RGB curves to RGB group
        curvesHSL->setParent(groupHSL); // add HSL curves to HSL group

        // Default curve
        for(unsigned int i = 0; i < nbCurvesRGB; ++i)
        {
            curvesRGB->addControlPoint(i, 0.0, 0.0, 0.0, false);
            curvesRGB->addControlPoint(i, 0.0, 0.2, 1.0, false);
            curvesRGB->addControlPoint(i, 0.0, 0.6, 1.0, false);
            curvesRGB->addControlPoint(i, 0.0, 1.0, 0.0, false);
        }
        for(unsigned int i = 0; i < nbCurvesHSL; ++i)
        {
            curvesHSL->addControlPoint(i, 0.0, 0.0, 0.0, false);
            curvesHSL->addControlPoint(i, 0.0, 0.2, 1.0, false);
            curvesHSL->addControlPoint(i, 0.0, 0.6, 1.0, false);
            curvesHSL->addControlPoint(i, 0.0, 1.0, 0.0, false);
        }

        // Channels checkboxes (RGB)
        OFX::BooleanParamDescriptor* boolR = desc.defineBooleanParam(kBoolRed);
        boolR->setDefault(false); // red channel is not selected by default
        boolR->setHint("Activate Red channel");
        boolR->setLayoutHint(OFX::eLayoutHintNoNewLine); // line is not finished
        boolR->setParent(groupRGB);
        // red multiplier
        OFX::DoubleParamDescriptor* redMultiplier = desc.defineDoubleParam(kMultiplierRed);
        redMultiplier->setLabel(kMultiplierLabel);
        redMultiplier->setHint("Determinate curve from selection precision.");
        redMultiplier->setRange(1, 1000);
        redMultiplier->setDisplayRange(0, 5);
        redMultiplier->setDefault(1);
        redMultiplier->setParent(groupRGB);

        OFX::BooleanParamDescriptor* boolG = desc.defineBooleanParam(kBoolGreen);
        boolG->setDefault(false); // green channel is not selected by default
        boolG->setHint("Activate Green channel");
        boolG->setLayoutHint(OFX::eLayoutHintNoNewLine); // line is not finished
        boolG->setParent(groupRGB);
        // green multiplier
        OFX::DoubleParamDescriptor* greenMultiplier = desc.defineDoubleParam(kMultiplierGreen);
        greenMultiplier->setLabel(kMultiplierLabel);
        greenMultiplier->setHint("Determinate curve from selection precision.");
        greenMultiplier->setRange(1, 1000);
        greenMultiplier->setDisplayRange(0, 5);
        greenMultiplier->setDefault(1);
        greenMultiplier->setParent(groupRGB);

        OFX::BooleanParamDescriptor* boolB = desc.defineBooleanParam(kBoolBlue);
        boolB->setHint("Activate Blue channel");
        boolB->setLayoutHint(OFX::eLayoutHintNoNewLine); // line is not finished
        boolB->setDefault(false); // blue channel is not selected by default
        boolB->setParent(groupRGB);
        // blue multiplier
        OFX::DoubleParamDescriptor* blueMultiplier = desc.defineDoubleParam(kMultiplierBlue);
        blueMultiplier->setLabel(kMultiplierLabel);
        blueMultiplier->setHint("Determinate curve from selection precision.");
        blueMultiplier->setRange(1, 1000);
        blueMultiplier->setDisplayRange(0, 5);
        blueMultiplier->setDefault(1);
        blueMultiplier->setParent(groupRGB);

        // Channels check box (HSL)
        OFX::BooleanParamDescriptor* boolH = desc.defineBooleanParam(kBoolHue);
        boolH->setDefault(false);
        boolH->setHint("Activate Hue channel");
        boolH->setLayoutHint(OFX::eLayoutHintNoNewLine); // line is not finished
        boolH->setParent(groupHSL);
        // Hue multiplier
        OFX::DoubleParamDescriptor* hueMultiplier = desc.defineDoubleParam(kMultiplierHue);
        hueMultiplier->setLabel(kMultiplierLabel);
        hueMultiplier->setHint("Determinate curve from selection precision.");
        hueMultiplier->setRange(1, 1000);
        hueMultiplier->setDisplayRange(0, 5);
        hueMultiplier->setDefault(1);
        hueMultiplier->setParent(groupHSL);

        OFX::BooleanParamDescriptor* boolS = desc.defineBooleanParam(kBoolSaturation);
        boolS->setDefault(false);
        boolS->setHint("Activate Saturation channel");
        boolS->setLayoutHint(OFX::eLayoutHintNoNewLine); // line is not finished
        boolS->setParent(groupHSL);
        // Saturation multiplier
        OFX::DoubleParamDescriptor* saturationMultiplier = desc.defineDoubleParam(kMultiplierSaturation);
        saturationMultiplier->setLabel(kMultiplierLabel);
        saturationMultiplier->setHint("Determinate curve from selection precision.");
        saturationMultiplier->setRange(1, 1000);
        saturationMultiplier->setDisplayRange(0, 5);
        saturationMultiplier->setDefault(1);
        saturationMultiplier->setParent(groupHSL);

        OFX::BooleanParamDescriptor* boolL = desc.defineBooleanParam(kBoolLightness);
        boolL->setHint("Activate Lightness channel");
        boolL->setLayoutHint(OFX::eLayoutHintNoNewLine); // line is not finished
        boolL->setDefault(false);
        boolL->setParent(groupHSL);
        // Lightness multiplier
        OFX::DoubleParamDescriptor* lightnessMultiplier = desc.defineDoubleParam(kMultiplierLightness);
        lightnessMultiplier->setLabel(kMultiplierLabel);
        lightnessMultiplier->setHint("Determinate curve from selection precision.");
        lightnessMultiplier->setRange(1, 1000);
        lightnessMultiplier->setDisplayRange(0, 5);
        lightnessMultiplier->setDefault(1);
        lightnessMultiplier->setParent(groupHSL);

        // Clean Button (RGB)
        OFX::PushButtonParamDescriptor* resetButtonRGB = desc.definePushButtonParam(kButtonResetRGB);
        resetButtonRGB->setLabel(kButtonResetRGBLabel);
        resetButtonRGB->setLayoutHint(OFX::eLayoutHintNoNewLine); // line is not finished
        resetButtonRGB->setHint(
            "Reset the selected RGB curves. \n Warning : the curves may not be refreshed click on overlay to refresh.");
        resetButtonRGB->setParent(groupRGB);

        // Selection To Curves Button (RGB)
        OFX::PushButtonParamDescriptor* selectionToCurveButtonRGB = desc.definePushButtonParam(kButtonSelectionToCurveRGB);
        selectionToCurveButtonRGB->setLabel(kButtonSelectionToCurveRGBLabel);
        selectionToCurveButtonRGB->setHint("Load selected RGB curves with selection data. \n Warning : the curves may not "
                                           "be refreshed click on overlay to refresh.");
        selectionToCurveButtonRGB->setParent(groupRGB);

        // Append selection to curves button (RGB)
        OFX::PushButtonParamDescriptor* appendSelectionToCurveRGB =
            desc.definePushButtonParam(kButtonAppendSelectionToCurveRGB);
        appendSelectionToCurveRGB->setLabel(kButtonAppendSelectionToCurveRGBLabel); // add label
        appendSelectionToCurveRGB->setHint("Append current selection to selected RGB channels"); // help
        appendSelectionToCurveRGB->setParent(groupRGB); // add to RGB group

        // Clean Button (HSL)
        OFX::PushButtonParamDescriptor* resetButtonHSL = desc.definePushButtonParam(kButtonResetHSL);
        resetButtonHSL->setLabel(kButtonResetHSLLabel);
        resetButtonHSL->setLayoutHint(OFX::eLayoutHintNoNewLine); // line is not finished
        resetButtonHSL->setHint(
            "Reset the selected HSL curves \n Warning : the curves may not be refreshed click on overlay to refresh.");
        resetButtonHSL->setParent(groupHSL);

        // Selection To Curves Button (HSL)
        OFX::PushButtonParamDescriptor* selectionToCurveButtonHSL = desc.definePushButtonParam(kButtonSelectionToCurveHSL);
        selectionToCurveButtonHSL->setLabel(kButtonSelectionToCurveHSLLabel);
        selectionToCurveButtonHSL->setHint("Load selected HSL curves with selection data. \n Warning : the curves may not "
                                           "be refreshed click on overlay to refresh.");
        selectionToCurveButtonHSL->setParent(groupHSL);

        // Append selection to curves button (HSL)
        OFX::PushButtonParamDescriptor* appendSelectionToCurveHSL =
            desc.definePushButtonParam(kButtonAppendSelectionToCurveHSL);
        appendSelectionToCurveHSL->setLabel(kButtonAppendSelectionToCurveHSLLabel); // add label
        appendSelectionToCurveHSL->setHint("Append current selection to selected HSL channels"); // help
        appendSelectionToCurveHSL->setParent(groupHSL); // add to HSL group

        // Close RGB group (group states by default on screen)
        groupRGB->setOpen(false);
        groupHSL->setOpen(true);
    }

    // Selection group
    {
        OFX::GroupParamDescriptor* groupSelection = desc.defineGroupParam(kGroupSelection);
        groupSelection->setLabel(kGroupSelectionLabel);
        groupSelection->setOpen(false);
        groupSelection->setAsTab();
        // display selection
        OFX::BooleanParamDescriptor* boolDisplaySelection = desc.defineBooleanParam(kBoolSelection);
        boolDisplaySelection->setDefault(true);
        boolDisplaySelection->setEvaluateOnChange(false); // don't need to recompute on change
        boolDisplaySelection->setHint("Display the selected zone on screen.");
        boolDisplaySelection->setParent(groupSelection);
        // clear selection
        OFX::PushButtonParamDescriptor* resetSelectionButton = desc.definePushButtonParam(kButtonResetSelection);
        resetSelectionButton->setLabel(kButtonResetSelectionLabel);
        resetSelectionButton->setHint("Reset user's selection.");
        resetSelectionButton->setParent(groupSelection);
        // selection mode
        OFX::ChoiceParamDescriptor* selectionMode = desc.defineChoiceParam(kSelectionModeListParamLabel);
        selectionMode->setLabel(kSelectionModeListParamLabel);
        selectionMode->setHint("Selection mode \n - unique : reset past selection before selection \n - additive : add "
                               "pixels to current selection \n -subtractive : remote pixel from current selection");
        selectionMode->appendOption(kSelectionModeListParamOpt2);
        selectionMode->appendOption(kSelectionModeListParamOpt1);
        selectionMode->appendOption(kSelectionModeListParamOpt3);
        selectionMode->setParent(groupSelection);
        // Precision of selection to curve
        OFX::IntParamDescriptor* precisionSelectionToCurve = desc.defineIntParam(kprecisionCurveFromSelection);
        precisionSelectionToCurve->setLabel(kprecisionCurveFromSelectionLabel);
        precisionSelectionToCurve->setHint("Determinate curve from selection precision.");
        precisionSelectionToCurve->setRange(1, 1000);
        precisionSelectionToCurve->setDisplayRange(1, 300.0);
        precisionSelectionToCurve->setDefault(curveFromSelection);
        precisionSelectionToCurve->setEvaluateOnChange(false); // don't need to recompute on change
        precisionSelectionToCurve->setParent(groupSelection);
    }

    // Histogram overlay group
    {
        OFX::GroupParamDescriptor* groupHistogramOverlay = desc.defineGroupParam(kGroupHistogramOverlay);
        groupHistogramOverlay->setLabel(kGroupHistogramOverlayLabel);
        groupHistogramOverlay->setOpen(true);
        groupHistogramOverlay->setAsTab();

        // Histogram display settings
        OFX::ChoiceParamDescriptor* gammaType = desc.defineChoiceParam(kHistoDisplayListParamLabel);
        gammaType->setLabel(kHistoDisplayListParamLabel);
        gammaType->setEvaluateOnChange(false); // don't need to recompute on change
        gammaType->setHint(
            "Histogram display \n -global : normalize all of channels \n -by channel : keep proportions between channels");
        gammaType->appendOption(kHistoDisplayListParamOpt2);
        gammaType->appendOption(kHistoDisplayListParamOpt1);
        gammaType->setParent(groupHistogramOverlay);

        // Clean all Button
        OFX::PushButtonParamDescriptor* resetButtonAll = desc.definePushButtonParam(kButtonResetAll);
        resetButtonAll->setLabel(kButtonResetAllLabel);
        resetButtonAll->setHint(
            "Reset all curves. \n Waring : the curves may not be refreshed click on overlay to refresh.");
        resetButtonAll->setParent(groupHistogramOverlay);
    }

    /// Advanced group
    {
        OFX::GroupParamDescriptor* groupAdvanced = desc.defineGroupParam(kGroupAdvanced);
        groupAdvanced->setLabel(kGroupAdvancedLabel);
        groupAdvanced->setOpen(false);
        groupAdvanced->setAsTab();

        // nbOfstep (advanced group)
        OFX::IntParamDescriptor* nbStepRange = desc.defineIntParam(knbStepRange);
        nbStepRange->setLabel(knbStepRangeLabel);
        nbStepRange->setHint("Determinate histogram overlay precision.");
        nbStepRange->setRange(1, 1000);
        nbStepRange->setDisplayRange(1, 600.0);
        nbStepRange->setDefault(255);
        nbStepRange->setEvaluateOnChange(false); // don't need to recompute on change
        nbStepRange->setParent(groupAdvanced);
        // selection multiplier (advanced group)
        OFX::DoubleParamDescriptor* selectionMultiplier = desc.defineDoubleParam(kselectionMultiplier);
        selectionMultiplier->setLabel(kselectionMultiplierLabel);
        selectionMultiplier->setHint("With high values, small selection are more visible.");
        selectionMultiplier->setRange(0.001, 1000.0);
        selectionMultiplier->setDisplayRange(0.0, 100.0);
        selectionMultiplier->setDefault(2.0);
        selectionMultiplier->setEvaluateOnChange(false); // don't need to recompute on change
        selectionMultiplier->setParent(groupAdvanced);

        // Refresh histograms overlay Button
        OFX::PushButtonParamDescriptor* refreshOverlayButton = desc.definePushButtonParam(kButtonRefreshOverlay);
        refreshOverlayButton->setLabel(kButtonRefreshOverlayLabel);
        refreshOverlayButton->setHint("Refresh histogram overlay.");
        refreshOverlayButton->setParent(groupAdvanced);

        // clamp values to 0 and 1
        OFX::BooleanParamDescriptor* clampCurveValues = desc.defineBooleanParam(kBoolClampValues);
        clampCurveValues->setHint("Clamp curve value : values superior to 1 or inferior to 0 will be clamp in process.");
        clampCurveValues->setDefault(true);
        clampCurveValues->setParent(groupAdvanced);
    }
    // Output settings
    OFX::ChoiceParamDescriptor* outputType = desc.defineChoiceParam(kOutputListParamLabel);
    outputType->setLabel(kOutputListParamLabel);
    outputType->setHint("Output type \n Alpha channel or Black and White");
    outputType->appendOption(kOutputListParamOpt1);
    outputType->appendOption(kOutputListParamOpt2);
    outputType->setLayoutHint(OFX::eLayoutHintNoNewLine); // line is not finished

    // Reverse mask
    OFX::BooleanParamDescriptor* boolReverseMask = desc.defineBooleanParam(kBoolReverseMask);
    boolReverseMask->setDefault(false);
    boolReverseMask->setHint("Revert alpha mask");
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* HistogramKeyerPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new HistogramKeyerPlugin(handle);
}
}
}
}
