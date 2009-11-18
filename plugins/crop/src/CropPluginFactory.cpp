/**
 * @file CropPluginFactory.cpp
 * @brief
 * @author
 * @date    01/10/09 14:14
 *
 */

#include "CropPlugin.hpp"
#include "CropMargin.hpp"
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace OFX {

static const bool   kSupportTiles           = true;
static const bool   kSupportTemporalAccess  = false;


mDeclarePluginFactory(CropPluginFactory, {}, {});

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
using namespace OFX;
void
CropPluginFactory::describe(OFX::ImageEffectDescriptor &desc)
{
    // basic labels
    desc.setLabels("volet", "volet",
                   "image volet");
    desc.setPluginGrouping("project");

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(eContextGeneral);
    desc.addSupportedContext(eContextFilter);

    // add supported pixel depths
    desc.addSupportedBitDepth(eBitDepthUByte);
    desc.addSupportedBitDepth(eBitDepthUShort);
    desc.addSupportedBitDepth(eBitDepthFloat);

    // set a few flags
    desc.setSingleInstance(false);
    desc.setHostFrameThreading(false);
    desc.setSupportsMultiResolution(false);
    desc.setSupportsTiles(kSupportTiles);
    desc.setTemporalClipAccess(false);
    desc.setRenderTwiceAlways(false);
    desc.setSupportsMultipleClipPARs(false);

    desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayDescriptor<CropMarginOverlay, CropMargin > ( ) );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void
CropPluginFactory::describeInContext(OFX::ImageEffectDescriptor &desc,
                                          OFX::ContextEnum context)
{
    OFX::ClipDescriptor *srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
    srcClip->addSupportedComponent( ePixelComponentRGBA );
    srcClip->addSupportedComponent( ePixelComponentAlpha );
    srcClip->setSupportsTiles( kSupportTiles );

    // Create the mandated output clip
    OFX::ClipDescriptor *dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
    dstClip->addSupportedComponent( ePixelComponentRGBA );
    dstClip->addSupportedComponent( ePixelComponentAlpha );
    dstClip->setSupportsTiles( kSupportTiles );

    OFX::ChoiceParamDescriptor *format = desc.defineChoiceParam( kParamFormats );
    format->setScriptName( "formats" );
    format->appendOption( "1 : 1,33 (4/3) bands" );
    format->appendOption( "1 : 1,77 (16/9e) bands" );
    format->appendOption( "1 : 1,85 bands" );
    format->appendOption( "1 : 2,35 (Cinemascope) bands" );
    format->appendOption( "1 : 2,40 bands" );
    format->setDefault( 0 );

    OFX::BooleanParamDescriptor* shape = desc.defineBooleanParam( kParamDisplayRect );
    shape->setDefault( false );

    OFX::BooleanParamDescriptor* anamorphic = desc.defineBooleanParam( kParamAnamorphic );
    anamorphic->setLabels( "Anamorphic", "Anamorphic", "Anamorphic (stretch)" );
    anamorphic->setDefault( false );
    anamorphic->setIsSecret( true );

    OFX::GroupParamDescriptor *bandsGroup   = desc.defineGroupParam( "Bands sizes" );
    OFX::IntParamDescriptor *upBand         = desc.defineIntParam( kParamUp );
    upBand->setScriptName( "upBand" );
    upBand->setParent( *bandsGroup );

    OFX::IntParamDescriptor *downBand   = desc.defineIntParam( kParamDown );
    downBand->setScriptName( "downBand" );
    downBand->setParent( *bandsGroup );

    OFX::IntParamDescriptor *leftBand   = desc.defineIntParam( kParamLeft );
    leftBand->setScriptName( "leftBand" );
    leftBand->setParent( *bandsGroup );

    OFX::IntParamDescriptor *rightBand  = desc.defineIntParam( kParamRight );
    rightBand->setScriptName( "rightBand" );
    rightBand->setParent( *bandsGroup );

    OFX::PushButtonParamDescriptor *helpButton = desc.definePushButtonParam( "Help" );
    helpButton->setScriptName( "&Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect*
CropPluginFactory::createInstance(OfxImageEffectHandle handle,
                                            OFX::ContextEnum context)
{
    return new CropPlugin(handle);
}

}

namespace OFX
{
    namespace Plugin 
    {
        void getPluginIDs(OFX::PluginFactoryArray &ids)
        {
            static OFX::CropPluginFactory p("fr.hd3d.tuttle.volet", 1, 0);
            ids.push_back(&p);
        }
    }
}
