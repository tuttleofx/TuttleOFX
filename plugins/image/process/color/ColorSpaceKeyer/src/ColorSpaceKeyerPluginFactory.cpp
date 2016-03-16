#include "ColorSpaceKeyerPluginFactory.hpp"
#include "ColorSpaceKeyerPlugin.hpp"
#include "ColorSpaceKeyerDefinitions.hpp"
#include "ColorSpaceKeyerOverlay.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle
{
namespace plugin
{
namespace colorSpaceKeyer
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ColorSpaceKeyerPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    // describe the plugin
    desc.setLabels("TuttleColorSpaceKeyer", "ColorSpaceKeyer", "ColorSpaceKeyer");
    desc.setPluginGrouping("tuttle/image/process/color");

    desc.setDescription("ColorSpaceKeyer\n"
                        "Plugin is used to ???.");

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(OFX::eContextFilter);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // plugin flags
    desc.setSupportsTiles(true);
    desc.setHostFrameThreading(false);

    // Don't declare the plugin as fully safe: It's not a safety problem, but we need to compute common data.
    // So it's not interesting to compute the same thing on multiple threads.
    //	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
    desc.setRenderThreadSafety(OFX::eRenderInstanceSafe);

    // Overlay class
    desc.setOverlayInteractDescriptor(new OFX::DefaultEffectOverlayWrap<ColorSpaceKeyerOverlayDescriptor>());
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ColorSpaceKeyerPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClip->setSupportsTiles(true);

    OFX::ClipDescriptor* strongSelectionClip = desc.defineClip(kClipColorSelection);
    strongSelectionClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    strongSelectionClip->setOptional(true);
    strongSelectionClip->setSupportsTiles(false);

    OFX::ClipDescriptor* spillSelectionClip = desc.defineClip(kClipSpillSelection);
    spillSelectionClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    spillSelectionClip->setOptional(true);
    spillSelectionClip->setSupportsTiles(false);

    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->setSupportsTiles(true);

    // Group display
    OFX::GroupParamDescriptor* groupDisplay = desc.defineGroupParam(kGroupDisplay);
    groupDisplay->setLabel(kGroupDisplayLabel);
    groupDisplay->setAsTab(); // current group is a tab

    // Group settings
    OFX::GroupParamDescriptor* groupSettings = desc.defineGroupParam(kGroupSettings);
    groupSettings->setLabel(kGroupSettingsLabel); // add label
    groupSettings->setAsTab(); // current group is a tab

    // Group process
    OFX::GroupParamDescriptor* groupProcess = desc.defineGroupParam(kGroupProcess);
    groupProcess->setLabel(kGroupProcessLabel); // add label
    groupProcess->setAsTab(); // current group is a tab

    // Check-box display
    OFX::BooleanParamDescriptor* pointCloudDisplay = desc.defineBooleanParam(kPointCloudDisplay);
    pointCloudDisplay->setLabel(kPointCloudDisplayLabel); // label
    pointCloudDisplay->setDefault(false); // display overlay by default
    pointCloudDisplay->setEvaluateOnChange(false); // Don't render on change
    pointCloudDisplay->setParent(groupDisplay); // add component to group display

    // Check-box discretization cloud point display
    OFX::BooleanParamDescriptor* pointCloudDiscretization = desc.defineBooleanParam(kBoolDiscretizationDisplay);
    pointCloudDiscretization->setLabel(kBoolDiscretizationDisplayLabel); // label
    pointCloudDiscretization->setDefault(false); // display overlay by default
    pointCloudDiscretization->setLayoutHint(OFX::eLayoutHintNoNewLine); // line is not finished
    pointCloudDiscretization->setEnabled(false); // Disabled by default (display cloud point is not selected)
    pointCloudDiscretization->setHint("Activate discretization point cloud.");
    pointCloudDiscretization->setEvaluateOnChange(false); // Don't render on change
    pointCloudDiscretization->setParent(groupDisplay); // add component to group Display

    // Discretization int range
    OFX::IntParamDescriptor* discretizationDisplay = desc.defineIntParam(kIntDiscretizationDisplay);
    discretizationDisplay->setLabel(kIntDiscretizationDisplayLabel); // label
    discretizationDisplay->setRange(1, 500); // value range
    discretizationDisplay->setDisplayRange(1, 30); // display value range
    discretizationDisplay->setDefault(10); // default value
    discretizationDisplay->setEnabled(false); // Disabled by default (display cloud point is not selected)
    discretizationDisplay->setHint("Change discretization point cloud step.");
    discretizationDisplay->setEvaluateOnChange(false); // Don't render on change
    discretizationDisplay->setParent(groupDisplay); // add component to group display

    // Push button to reset transformation parameters
    OFX::PushButtonParamDescriptor* resetTransformationParameters =
        desc.definePushButtonParam(kPushButtonResetTransformationParameters);
    resetTransformationParameters->setLabel(kPushButtonResetTransformationParametersLabel); // label
    resetTransformationParameters->setHint("Reset view parameters"); // help
    resetTransformationParameters->setParent(groupDisplay); // add component to group display

    // Number of divison Geodesic form
    OFX::IntParamDescriptor* nbDivisionsGeodesicForm = desc.defineIntParam(kIntNumberOfDivisonGeodesicForm);
    nbDivisionsGeodesicForm->setLabel(kIntNumberOfDivisonGeodesicFormLabel); // label
    nbDivisionsGeodesicForm->setRange(2, 50); // value range
    nbDivisionsGeodesicForm->setDisplayRange(2, 15); // display range values
    nbDivisionsGeodesicForm->setDefault(4); // default value
    nbDivisionsGeodesicForm->setHint("Change precision of treatment (can make process slower)"); // help
    nbDivisionsGeodesicForm->setParent(groupSettings); // add component to settings group

    // Selection average mode
    OFX::ChoiceParamDescriptor* averageMode = desc.defineChoiceParam(kColorAverageMode);
    averageMode->setLabel(kColorAverageModeLabel);
    averageMode->setHint(
        "Average mode :\n -automatic : average of color selection \n -manual : choose average separately from selection");
    averageMode->appendOption(kColorAverageMode1);
    averageMode->appendOption(kColorAverageMode2);
    averageMode->setParent(groupSettings);

    // Selection average selection
    OFX::RGBAParamDescriptor* colorAverage = desc.defineRGBAParam(kColorAverageSelection);
    colorAverage->setLabel(kColorAverageSelectionLabel); // label
    colorAverage->setHint("Select the average of the selection"); // help
    colorAverage->setEnabled(false); // Disabled by default
    colorAverage->setLayoutHint(OFX::eLayoutHintNoNewLine); // line is not finished
    colorAverage->setParent(groupSettings); // add to settings group

    // Selection average compute
    OFX::PushButtonParamDescriptor* colorAverageCompute = desc.definePushButtonParam(kColorAverageComputing);
    colorAverageCompute->setLabel(kColorAverageComputingLabel); // label
    colorAverageCompute->setEnabled(false); // Disabled by default
    colorAverageCompute->setParent(groupSettings); // add to settings group

    // Check-box only selection color
    OFX::BooleanParamDescriptor* onlySelectionColor = desc.defineBooleanParam(kBoolOnlySelection);
    onlySelectionColor->setLabel(kBoolOnlySelectionLabel); // add label
    onlySelectionColor->setDefault(false); // check box is not checked by default
    onlySelectionColor->setEvaluateOnChange(false); // don't need to recompute on change
    onlySelectionColor->setHint("see color process form"); // help
    onlySelectionColor->setLayoutHint(OFX::eLayoutHintNoNewLine); // line is not finished
    onlySelectionColor->setParent(groupDisplay); // add to display group

    // Check-box only selection color
    OFX::BooleanParamDescriptor* displaySpillGF = desc.defineBooleanParam(kBoolDisplaySpillGF);
    displaySpillGF->setLabel(kBoolDisplaySpillGFLabel); // add label
    displaySpillGF->setDefault(false); // check box is not checked by default
    displaySpillGF->setEvaluateOnChange(false); // don't need to recompute on change
    displaySpillGF->setHint("see spill process form"); // help
    displaySpillGF->setParent(groupDisplay); // add to display group

    // Check box see color selection
    OFX::BooleanParamDescriptor* seeSelection = desc.defineBooleanParam(kBoolColorSelectionDisplay);
    seeSelection->setLabel(kBoolColorSelectionDisplayLabel); // add label
    seeSelection->setDefault(false); // check box is checked by default
    seeSelection->setEvaluateOnChange(false); // don't need to recompute on change
    seeSelection->setHint("Do not see color selection"); // help
    seeSelection->setLayoutHint(OFX::eLayoutHintNoNewLine); // line is not finished
    seeSelection->setParent(groupDisplay); // add to display group

    // Check box see spill selection
    OFX::BooleanParamDescriptor* seeSpillSelection = desc.defineBooleanParam(kBoolSpillSelectionDisplay);
    seeSpillSelection->setLabel(kBoolSpillSelectionDisplayLabel); // add label
    seeSpillSelection->setDefault(false); // check box is checked by default
    seeSpillSelection->setEvaluateOnChange(false); // don't need to recompute on change
    seeSpillSelection->setHint("Do not see spill selection"); // help
    seeSpillSelection->setParent(groupDisplay); // add to display group

    // Double parameters scale (scale geodesic form)
    OFX::DoubleParamDescriptor* scaleGF = desc.defineDoubleParam(kDoubleScaleGeodesicForm);
    scaleGF->setLabel(kDoubleScaleGeodesicFormLabel); // add label
    scaleGF->setDefault(1.0); // default value
    scaleGF->setRange(0, 5.0); // scale range
    scaleGF->setDisplayRange(0, 2); // set display range
    scaleGF->setHint("Scale geodesic form"); // help
    scaleGF->setParent(groupProcess); // add to process group
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ColorSpaceKeyerPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new ColorSpaceKeyerPlugin(handle);
}
}
}
}
