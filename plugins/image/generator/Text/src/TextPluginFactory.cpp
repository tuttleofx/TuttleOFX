#include "TextPluginFactory.hpp"
#include "TextPlugin.hpp"
#include "TextDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/context/GeneratorPluginFactory.hpp>

#ifndef __WINDOWS__
#include <fontconfig/fontconfig.h>
#endif

#include <iostream>

#include <boost/algorithm/string.hpp>

#include <limits>

namespace tuttle
{
namespace plugin
{
namespace text
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void TextPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleText", "Text", "Text");
    desc.setPluginGrouping("tuttle/image/generator");

    desc.setDescription("Text generator\n"
                        "Plugin is used to burn text on images.");

    // add the supported contexts
    desc.addSupportedContext(OFX::eContextGenerator);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // plugin flags
    desc.setRenderThreadSafety(OFX::eRenderFullySafe);
    desc.setHostFrameThreading(false);
    desc.setSupportsMultiResolution(false);
    desc.setSupportsMultipleClipDepths(true);
    desc.setSupportsTiles(kSupportTiles);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void TextPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    describeGeneratorParamsInContext(desc, context);

    OFX::StringParamDescriptor* text = desc.defineStringParam(kParamText);
    text->setLabel("Text");
    text->setStringType(OFX::eStringTypeMultiLine);

    OFX::StringParamDescriptor* fontFile = desc.defineStringParam(kParamFontPath);
    fontFile->setStringType(OFX::eStringTypeFilePath);
    fontFile->setHint("When a font file path is fill, font & bold and italic options are not available.");

#ifndef __WINDOWS__
    OFX::ChoiceParamDescriptor* font = desc.defineChoiceParam(kParamFont);

    FcInit();
    FcConfig* config = FcInitLoadConfigAndFonts();
    FcChar8* s;
    FcPattern* p =
        FcPatternBuild(NULL, FC_WEIGHT, FcTypeInteger, FC_WEIGHT_BOLD, FC_SLANT, FcTypeInteger, FC_SLANT_ITALIC, NULL);

    FcObjectSet* os = FcObjectSetBuild(FC_FAMILY, NULL);
    FcFontSet* fs = FcFontList(config, p, os);

    for(int i = 0; fs && i < fs->nfont; i++)
    {
        FcPattern* fcFont = fs->fonts[i];
        s = FcNameUnparse(fcFont);

        std::ostringstream stream;
        stream << s;
        std::string id = stream.str();
        boost::algorithm::to_lower(id);
        boost::replace_all(id, " ", "_");

        font->appendOption(id, stream.str());

        // if available, set Arial, Courier or then Utopia font as default
        if(!strcmp((char*)s, "Arial"))
            font->setDefault(i);
        else if(!strcmp((char*)s, "Courier"))
            font->setDefault(i);
        else if(!strcmp((char*)s, "Utopia"))
            font->setDefault(i);
    }
    font->setHint("Select the font.");
    font->setLabel("Font");
#endif
    OFX::IntParamDescriptor* size = desc.defineIntParam(kParamSize);
    size->setLabel("Font Size");
    size->setDefault(18);
    size->setRange(0, std::numeric_limits<int>::max());
    size->setDisplayRange(0, 60);
    /*
            OFX::DoubleParamDescriptor* ratio = desc.defineDoubleParam( kParamRatio );
            ratio->setLabel( "Ratio" );
            ratio->setRange( 0.0, std::numeric_limits<double>::max() );
            ratio->setDisplayRange( 0.0, 2.0 );
            ratio->setDefault( 1.0 );
    */
    OFX::RGBAParamDescriptor* color = desc.defineRGBAParam(kParamColor);
    color->setLabel("Color");
    color->setDefault(1.0, 1.0, 1.0, 1.0);

    OFX::RGBAParamDescriptor* backgroundColor = desc.defineRGBAParam(kParamBackgroundColor);
    backgroundColor->setLabel("Background Color");
    backgroundColor->setDefault(0.0, 0.0, 0.0, 0.0);

    OFX::Double2DParamDescriptor* position = desc.defineDouble2DParam(kParamPosition);
    position->setLabel("Position");
    position->setDefault(0.0, 0.0);

    OFX::DoubleParamDescriptor* letterSpacing = desc.defineDoubleParam(kParamLetterSpacing);
    letterSpacing->setLabel("Letter spacing");
    letterSpacing->setDisplayRange(-10.0, 10.0);
    letterSpacing->setDefault(0.0);

    OFX::ChoiceParamDescriptor* vAlign = desc.defineChoiceParam(kParamVAlign);
    vAlign->setLabel("Vertically align");
    vAlign->appendOption(kParamVAlignTop);
    vAlign->appendOption(kParamVAlignCenter);
    vAlign->appendOption(kParamVAlignBottom);
    vAlign->setDefault(eParamVAlignCenter);

    OFX::ChoiceParamDescriptor* hAlign = desc.defineChoiceParam(kParamHAlign);
    hAlign->setLabel("Horizontally align");
    hAlign->appendOption(kParamHAlignLeft);
    hAlign->appendOption(kParamHAlignCenter);
    hAlign->appendOption(kParamHAlignRight);
    hAlign->setDefault(eParamHAlignCenter);

    OFX::BooleanParamDescriptor* verticalFlip = desc.defineBooleanParam(kParamVerticalFlip);
    verticalFlip->setLabel("Vertical flip");
    verticalFlip->setDefault(false);
    verticalFlip->setAnimates(false);
    verticalFlip->setHint("Some hosts use inverted images, so you can correct this problem using this flag.");

    OFX::BooleanParamDescriptor* italic = desc.defineBooleanParam(kParamItalic);
    italic->setLabel("Italic");
    italic->setDefault(false);

    OFX::BooleanParamDescriptor* bold = desc.defineBooleanParam(kParamBold);
    bold->setLabel("Bold");
    bold->setDefault(false);

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
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* TextPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new TextPlugin(handle);
}
}
}
}
