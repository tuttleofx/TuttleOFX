#include "MergePluginFactory.hpp"
#include "MergePlugin.hpp"
#include "MergeDefinitions.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle
{
namespace plugin
{
namespace merge
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void MergePluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleMerge", "Merge", "Merge two images");
    desc.setPluginGrouping("tuttle/image/process/transition");

    desc.setDescription("Clip merging\n"
                        "Plugin is used to merge two clips A and B.");

    // add the supported contexts
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // plugin flags
    desc.setSupportsTiles(kSupportTiles);
    desc.setRenderThreadSafety(OFX::eRenderFullySafe);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void MergePluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ClipDescriptor* srcClipB = desc.defineClip(kParamSourceB);
    srcClipB->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClipB->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClipB->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClipB->setSupportsTiles(kSupportTiles);
    srcClipB->setOptional(false);

    OFX::ClipDescriptor* srcClipA = desc.defineClip(kParamSourceA);
    srcClipA->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClipA->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClipA->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClipA->setSupportsTiles(kSupportTiles);
    srcClipA->setOptional(false);

    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    // Define some merging function
    OFX::ChoiceParamDescriptor* mergeFunction = desc.defineChoiceParam(kParamFunction);
    mergeFunction->setLabels(kParamFunctionLabel, kParamFunctionLabel, kParamFunctionLabel);
    mergeFunction->appendOption("atop", "atop: Ab+B(1-a)");
    mergeFunction->appendOption("average", "average: (A+B)/2");
    mergeFunction->appendOption("color", "color: hue from B, saturation from B, lightness from A");
    mergeFunction->appendOption("color-burn", "color-burn: darken B towards A");
    mergeFunction->appendOption("color dodge inversed", "color dodge inversed: brighten B towards A");
    mergeFunction->appendOption("conjoint-over", "conjoint-over: A+B(1-a)/b, A if a > b");
    mergeFunction->appendOption("copy", "copy: A");
    mergeFunction->appendOption("difference", "difference: abs(A-B)");
    mergeFunction->appendOption("disjoint-over", "disjoint-over: A+B(1-a)/b, A+B if a+b < 1");
    mergeFunction->appendOption("divide", "divide: A/B, 0 if A < 0 and B < 0");
    mergeFunction->appendOption("exclusion", "exclusion: A+B-2AB");
    mergeFunction->appendOption("freeze", "freeze: 1-sqrt(1-A)/B");
    mergeFunction->appendOption("from", "from: B-A");
    mergeFunction->appendOption("geometric", "geometric: 2AB/(A+B)");
    mergeFunction->appendOption("hard-light", "hard-light: multiply if A < 0.5, screen if A > 0.5");
    mergeFunction->appendOption("hypot", "hypot: sqrt(A*A+B*B)");
    mergeFunction->appendOption("in", "in: Ab");
    mergeFunction->appendOption("interpolated", "interpolated: (like average but better and slower)");
    mergeFunction->appendOption("mask", "mask: Ba");
    mergeFunction->appendOption("matte", "matte: Aa + B(1-a) (unpremultiplied over)");
    mergeFunction->appendOption("lighten", "lighten: max(A, B)");
    mergeFunction->appendOption("darken", "darken: min(A, B)");
    mergeFunction->appendOption("minus", "minus: A-B");
    mergeFunction->appendOption("multiply", "multiply: AB, 0 if A < 0 and B < 0");
    mergeFunction->appendOption("out", "out: A(1-b)");
    mergeFunction->appendOption("over", "over: A+B(1-a)");
    mergeFunction->appendOption("overlay", "overlay: multiply if B<0.5, screen if B>0.5");
    mergeFunction->appendOption("pinlight", "pinlight: if B >= 0.5 then max(A, 2*B - 1), min(A, B * 2.0 ) else");
    mergeFunction->appendOption("plus", "plus: A+B");
    mergeFunction->appendOption("reflect", "reflect: a² / (1 - b)");
    mergeFunction->appendOption("screen", "screen: A+B-AB");
    mergeFunction->appendOption("stencil", "stencil: B(1-a)");
    mergeFunction->appendOption("under", "under: A(1-b)+B");
    mergeFunction->appendOption("xor", "xor: A(1-b)+B(1-a)");
    mergeFunction->setDefault(eParamMergePlus);

    OFX::Int2DParamDescriptor* offsetA = desc.defineInt2DParam(kParamOffsetA);
    offsetA->setLabel("A offset");
    offsetA->setDefault(0, 0);
    offsetA->setDisplayRange(0, 0, 300, 300);

    OFX::Int2DParamDescriptor* offsetB = desc.defineInt2DParam(kParamOffsetB);
    offsetB->setLabel("B offset");
    offsetB->setDefault(0, 0);
    offsetB->setDisplayRange(0, 0, 300, 300);

    OFX::ChoiceParamDescriptor* rod = desc.defineChoiceParam(kParamRod);
    rod->appendOption(kParamRodIntersect);
    rod->appendOption(kParamRodUnion);
    rod->appendOption(kParamRodA);
    rod->appendOption(kParamRodB);
    rod->setDefault(eParamRodIntersect);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* MergePluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new MergePlugin(handle);
}
}
}
}
