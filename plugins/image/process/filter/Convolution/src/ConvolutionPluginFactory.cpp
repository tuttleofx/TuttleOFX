#include "ConvolutionPluginFactory.hpp"
#include "ConvolutionPlugin.hpp"
#include "ConvolutionDefinitions.hpp"

#include <ofxImageEffect.h>

namespace tuttle
{
namespace plugin
{
namespace convolution
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ConvolutionPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleConvolution", "Convolution", "Convolution");
    desc.setPluginGrouping("tuttle/image/process/filter");

    desc.setDescription("A convolution is defined as the integral of the product of the two functions"
                        "after one is reversed and shifted."
                        "\n"
                        "\n"
                        "In mathematics and, in particular, functional analysis, convolution is a"
                        "mathematical operation on two functions f and g, producing a third function"
                        "that is typically viewed as a modified version of one of the original functions. "
                        "Convolution is similar to cross-correlation. It has applications that include"
                        "probability, statistics, computer vision, image and signal processing,"
                        "electrical engineering, and differential equations."
                        "\n"
                        "The convolution can be defined for functions on groups other than Euclidean"
                        "space. In particular, the circular convolution can be defined for periodic"
                        "functions (that is, functions on the circle), and the discrete convolution"
                        "can be defined for functions on the set of integers. These generalizations of"
                        "the convolution have applications in the field of numerical analysis and"
                        "numerical linear algebra, and in the design and implementation of finite"
                        "impulse response filters in signal processing."
                        "\n"
                        "\n"
                        "http://en.wikipedia.org/wiki/Convolution");

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
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ConvolutionPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClip->setSupportsTiles(kSupportTiles);

    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    OFX::Int2DParamDescriptor* size = desc.defineInt2DParam(kParamSize);
    size->setLabel("Size");
    size->setDefault(3, 3);
    //	size->setIncrement( 2, 2 );
    size->setRange(3, 3, kParamSizeMax, kParamSizeMax);

    OFX::ChoiceParamDescriptor* border = desc.defineChoiceParam(kParamBorder);
    border->setLabel("Border");
    border->appendOption(kParamBorderMirror);
    border->appendOption(kParamBorderConstant);
    border->appendOption(kParamBorderBlack);
    border->appendOption(kParamBorderPadded);

    for(unsigned int y = 0; y < kParamSizeMax; ++y)
    {
        for(unsigned int x = 0; x < kParamSizeMax; ++x)
        {
            const std::string name(getCoefName(y, x));
            OFX::DoubleParamDescriptor* coef = desc.defineDoubleParam(name);
            coef->setLabel(name);
            coef->setDisplayRange(-10.0, 10.0);
            coef->setDefault(0.0);
        }
    }
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ConvolutionPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new ConvolutionPlugin(handle);
}
}
}
}
