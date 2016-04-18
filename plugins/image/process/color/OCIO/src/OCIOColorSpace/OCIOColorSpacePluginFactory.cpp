#include "OCIOColorSpacePluginFactory.hpp"
#include "OCIOColorSpacePlugin.hpp"
#include "OCIOColorSpaceDefinitions.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <OpenColorIO/OpenColorIO.h>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace OCIO = OCIO_NAMESPACE;

namespace tuttle
{
namespace plugin
{
namespace ocio
{
namespace colorspace
{

bool global_wasOCIOVarFund;
/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void OCIOColorSpacePluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleOcioColorSpace", "OCIOColorSpace",
                   "ColorSpace transformation using OpenColorIO configuration file");
    desc.setPluginGrouping("tuttle/image/process/color/ocio");

    desc.setDescription("OCIO ColorSpace\n"
                        "\n"
                        "Config filename ");

    // add the supported contexts
    desc.addSupportedContext(OFX::eContextGeneral);
    desc.addSupportedContext(OFX::eContextFilter);

    // add supported pixel depths
    //	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    //	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    desc.setSupportsTiles(kSupportTiles);
    desc.setRenderThreadSafety(OFX::eRenderFullySafe);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */

void OCIOColorSpacePluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA); //@todo mfe, test this
    // srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );//@todo mfe, add and test this (cf OCIOPlugin.cpp, render)
    srcClip->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClip->setSupportsTiles(kSupportTiles);

    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    // dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->setSupportsTiles(kSupportTiles);

    // Controls

    OFX::StringParamDescriptor* filename = desc.defineStringParam(kTuttlePluginFilename);
    char* file = std::getenv("OCIO");

    filename->setEnabled(false);
    filename->setLabel(kTuttlePluginFilenameLabel);

    OFX::ChoiceParamDescriptor* inputSpace = desc.defineChoiceParam(kParamInputSpace);
    inputSpace->setLabel("Input Space");
    //////
    OFX::ChoiceParamDescriptor* outputSpace = desc.defineChoiceParam(kParamOutputSpace);
    outputSpace->setLabel("Output Space");

    if(file == NULL)
    {
        filename->setDefault("WARNING : You must set an OCIO environnement variable");
        inputSpace->setEnabled(false);
        outputSpace->setEnabled(false);
        global_wasOCIOVarFund = false;
        return;
    }
    global_wasOCIOVarFund = true;
    // filename->setStringType(OFX::eStringTypeFilePath);
    filename->setDefault(file);
    // Add choices
    OCIO::ConstConfigRcPtr config = OCIO::Config::CreateFromFile(file);
    for(int i = 0; i < config->getNumColorSpaces(); i++)
    {
        std::string csname = config->getColorSpaceNameByIndex(i);
        inputSpace->appendOption(csname);
        outputSpace->appendOption(csname);
    }
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* OCIOColorSpacePluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new OCIOColorSpacePlugin(handle, global_wasOCIOVarFund);
}
}
}
}
}
