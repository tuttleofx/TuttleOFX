#include "WarpPluginFactory.hpp"
#include "WarpPlugin.hpp"
#include "WarpDefinitions.hpp"
#include "WarpOverlayInteract.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <limits>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>

namespace tuttle
{
namespace plugin
{
namespace warp
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void WarpPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleWarp", "Warp", "Warp");
    desc.setPluginGrouping("tuttle/image/process/geometry");

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(OFX::eContextFilter);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // plugin flags
    desc.setSupportsTiles(false);
    desc.setOverlayInteractDescriptor(new OFX::DefaultEffectOverlayWrap<WarpEffectOverlayDescriptor>());
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void WarpPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClip->setSupportsTiles(false);

    OFX::ClipDescriptor* srcBClip = desc.defineClip(kClipSourceB);
    srcBClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcBClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcBClip->setOptional(true);
    srcBClip->setSupportsTiles(false);

    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);

    //////////////////// Options ////////////////////

    OFX::PushButtonParamDescriptor* reset = desc.definePushButtonParam(kParamReset);
    reset->setLabel("Reset");

    OFX::PushButtonParamDescriptor* setKey = desc.definePushButtonParam(kParamSetKey);
    setKey->setLabel("SetKey");

    OFX::PushButtonParamDescriptor* nextCurve = desc.definePushButtonParam(kParamNextCurve);
    nextCurve->setLabel("NextCurve");

    OFX::DoubleParamDescriptor* transition = desc.defineDoubleParam(kParamTransition);
    transition->setLabel("Transition");
    transition->setHint("Coefficient de transition");
    transition->setDefault(1.0);
    transition->setRange(0.0, 1.0);
    transition->setDisplayRange(0.0, 1.0);

    // Settings
    {
        OFX::GroupParamDescriptor* groupSettings = desc.defineGroupParam(kParamGroupSettings);
        groupSettings->setLabel("Settings");

        OFX::ChoiceParamDescriptor* method = desc.defineChoiceParam(kParamMethod);
        method->setLabel("Method");
        method->appendOption(kParamMethodCreation);
        method->appendOption(kParamMethodDelete);
        method->appendOption(kParamMethodMove);
        method->setDefault(0);
        method->setHint("Points method");
        method->setEvaluateOnChange(false);
        method->setParent(groupSettings);

        OFX::IntParamDescriptor* nbPoints = desc.defineIntParam(kParamNbPoints);
        nbPoints->setDefault(0);
        nbPoints->setRange(0, kMaxNbPoints - 1);
        nbPoints->setIsSecret(true);
        nbPoints->setParent(groupSettings);

        OFX::BooleanParamDescriptor* curveBegin[kMaxNbPoints];
        for(std::size_t cptCBegin = 0; cptCBegin < kMaxNbPoints; ++cptCBegin)
        {
            std::string resultCBegin = boost::lexical_cast<std::string>(cptCBegin);
            curveBegin[cptCBegin] = desc.defineBooleanParam(kParamCurveBegin + resultCBegin);
            curveBegin[cptCBegin]->setDefault(false);
            curveBegin[cptCBegin]->setParent(groupSettings);
            curveBegin[cptCBegin]->setIsSecret(true);
        }

        OFX::BooleanParamDescriptor* inverse = desc.defineBooleanParam(kParamInverse);
        inverse->setLabel("Inverse");
        inverse->setDefault(false);
        inverse->setParent(groupSettings);

        OFX::DoubleParamDescriptor* rigidity = desc.defineDoubleParam(kParamRigiditeTPS);
        rigidity->setLabel("Rigidity");
        rigidity->setHint("TPS Rigidity coefficient");
        rigidity->setDefault(0.0);
        //	rigidity->setRange( 0.0, std::numeric_limits<double>::max( ) );
        rigidity->setDisplayRange(0.0, 10.0);
        rigidity->setParent(groupSettings);

        OFX::IntParamDescriptor* nbPointsBezier = desc.defineIntParam(kParamNbPointsBezier);
        nbPointsBezier->setLabel("Bezier");
        nbPointsBezier->setHint("Nombre de points dessinant la courbe de bezier");
        nbPointsBezier->setDefault(5);
        nbPointsBezier->setRange(0, std::numeric_limits<int>::max());
        nbPointsBezier->setDisplayRange(1, 20);
        nbPointsBezier->setParent(groupSettings);

        // Overlay Points et tangentes
        OFX::GroupParamDescriptor* groupOverlay = desc.defineGroupParam(kParamGroupOverlay);
        groupOverlay->setLabel("Overlay points et tangentes");

        OFX::BooleanParamDescriptor* overlay = desc.defineBooleanParam(kParamOverlay);
        overlay->setLabel("Overlay");
        overlay->setHint("Affiche la scene entière ou non");
        overlay->setDefault(true);
        overlay->setEvaluateOnChange(false);
        overlay->setParent(groupOverlay);

        OFX::BooleanParamDescriptor* overlayIn = desc.defineBooleanParam(kParamOverlayIn);
        overlayIn->setLabel("Points In");
        overlayIn->setHint("Affiche les points d'entrée sur la scène");
        overlayIn->setDefault(true);
        overlayIn->setEvaluateOnChange(false);
        overlayIn->setParent(groupOverlay);

        OFX::BooleanParamDescriptor* overlayTgtIn = desc.defineBooleanParam(kParamOverlayTgtIn);
        overlayTgtIn->setLabel("Bezier et tangentes In");
        overlayTgtIn->setHint("Affiche la courbe de Bezier et ses tangentes en fonction des points In");
        overlayTgtIn->setDefault(true);
        overlayTgtIn->setEvaluateOnChange(false);
        overlayTgtIn->setParent(groupOverlay);

        OFX::BooleanParamDescriptor* overlayOut = desc.defineBooleanParam(kParamOverlayOut);
        overlayOut->setLabel("Points Out");
        overlayOut->setHint("Affiche les points de sortie sur la scène");
        overlayOut->setDefault(false);
        overlayOut->setEvaluateOnChange(false);
        overlayOut->setParent(groupOverlay);

        OFX::BooleanParamDescriptor* overlayTgtOut = desc.defineBooleanParam(kParamOverlayTgtOut);
        overlayTgtOut->setLabel("Bezier et tangentes Out");
        overlayTgtOut->setHint("Affiche la courbe de Bezier et ses tangentes en fonction des points Out");
        overlayTgtOut->setDefault(false);
        overlayTgtOut->setEvaluateOnChange(false);
        overlayTgtOut->setParent(groupOverlay);
    }

    //////////////////// IN Points ////////////////////
    {
        OFX::GroupParamDescriptor* groupIn = desc.defineGroupParam(kParamGroupIn);
        groupIn->setLabel("Input points");
        groupIn->setOpen(false);

        OFX::RGBParamDescriptor* ouverlayInColor = desc.defineRGBParam(kParamOverlayInColor);
        ouverlayInColor->setLabel("Color");
        ouverlayInColor->setHint("Input point overlay color");
        ouverlayInColor->setDefault(1.0, 0.0, 0.0);
        ouverlayInColor->setEvaluateOnChange(false);
        ouverlayInColor->setParent(groupIn);

        OFX::Double2DParamDescriptor* pIn[kMaxNbPoints];
        for(std::size_t cptIn = 0; cptIn < kMaxNbPoints; ++cptIn)
        {
            std::string resultIn = boost::lexical_cast<std::string>(cptIn);
            pIn[cptIn] = desc.defineDouble2DParam(kParamPointIn + resultIn);
            pIn[cptIn]->setLabel("In " + resultIn);
            pIn[cptIn]->setHint("Input point " + resultIn);
            pIn[cptIn]->setDefault(positionOrigine, positionOrigine);
            pIn[cptIn]->setParent(groupIn);
        }
    }

    //////////////////// TGT Points IN////////////////////
    {
        OFX::GroupParamDescriptor* groupTgtIn = desc.defineGroupParam(kParamGroupTgtIn);
        groupTgtIn->setLabel("Tangente points In");
        groupTgtIn->setOpen(false);

        OFX::RGBParamDescriptor* ouverlayTgtInColor = desc.defineRGBParam(kParamOverlayTgtInColor);
        ouverlayTgtInColor->setLabel("Color In");
        ouverlayTgtInColor->setHint("Tangente point overlay In color");
        ouverlayTgtInColor->setDefault(0.95, 0.4, 0.4);
        ouverlayTgtInColor->setEvaluateOnChange(false);
        ouverlayTgtInColor->setParent(groupTgtIn);

        OFX::Double2DParamDescriptor* pTgtIn[kMaxNbPoints * 2];
        for(std::size_t cptTgtIn = 0; cptTgtIn < kMaxNbPoints * 2; ++cptTgtIn)
        {
            std::string resultTgtIn = boost::lexical_cast<std::string>(cptTgtIn);
            pTgtIn[cptTgtIn] = desc.defineDouble2DParam(kParamPointTgtIn + resultTgtIn);
            pTgtIn[cptTgtIn]->setLabel("Tgt In " + resultTgtIn);
            pTgtIn[cptTgtIn]->setHint("Tgt point In " + resultTgtIn);
            pTgtIn[cptTgtIn]->setDefault(positionOrigine, positionOrigine);
            pTgtIn[cptTgtIn]->setParent(groupTgtIn);
        }

        //////////////////// OUT Points ////////////////////
        OFX::GroupParamDescriptor* groupOut = desc.defineGroupParam(kParamGroupOut);
        groupOut->setLabel("Output points");
        groupOut->setOpen(false);

        OFX::RGBParamDescriptor* ouverlayOutColor = desc.defineRGBParam(kParamOverlayOutColor);
        ouverlayOutColor->setLabel("Color");
        ouverlayOutColor->setHint("Output point overlay color");
        ouverlayOutColor->setDefault(0.0, 0.0, 1.0);
        ouverlayOutColor->setEvaluateOnChange(false);
        ouverlayOutColor->setParent(groupOut);

        OFX::Double2DParamDescriptor* pOut[kMaxNbPoints];
        for(std::size_t cptOut = 0; cptOut < kMaxNbPoints; ++cptOut)
        {
            std::string resultOut = boost::lexical_cast<std::string>(cptOut);
            pOut[cptOut] = desc.defineDouble2DParam(kParamPointOut + resultOut);
            pOut[cptOut]->setLabel("In " + resultOut);
            pOut[cptOut]->setHint("Input point " + resultOut);
            pOut[cptOut]->setDefault(positionOrigine, positionOrigine);
            pOut[cptOut]->setParent(groupOut);
        }
    }

    //////////////////// TGT Points Out////////////////////
    {
        OFX::GroupParamDescriptor* groupTgtOut = desc.defineGroupParam(kParamGroupTgtOut);
        groupTgtOut->setLabel("Tangente points Out");
        groupTgtOut->setOpen(false);

        OFX::RGBParamDescriptor* ouverlayTgtOutColor = desc.defineRGBParam(kParamOverlayTgtOutColor);
        ouverlayTgtOutColor->setLabel("Color Out");
        ouverlayTgtOutColor->setHint("Tangente point overlay Out color");
        ouverlayTgtOutColor->setDefault(0.2, 0.45, 0.95);
        ouverlayTgtOutColor->setEvaluateOnChange(false);
        ouverlayTgtOutColor->setParent(groupTgtOut);

        OFX::Double2DParamDescriptor* pTgtOut[kMaxNbPoints * 2];
        for(std::size_t cptTgtOut = 0; cptTgtOut < kMaxNbPoints * 2; ++cptTgtOut)
        {
            std::string resultTgtOut = boost::lexical_cast<std::string>(cptTgtOut);
            pTgtOut[cptTgtOut] = desc.defineDouble2DParam(kParamPointTgtOut + resultTgtOut);
            pTgtOut[cptTgtOut]->setLabel("Tgt Out " + resultTgtOut);
            pTgtOut[cptTgtOut]->setHint("Tgt point Out " + resultTgtOut);
            pTgtOut[cptTgtOut]->setDefault(positionOrigine, positionOrigine);
            pTgtOut[cptTgtOut]->setParent(groupTgtOut);
        }
    }
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* WarpPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new WarpPlugin(handle);
}
}
}
}
