/**
 * @file MergePluginFactory.cpp
 * @brief
 * @author
 * @date    27/11/09 13:42
 *
 */

#include "MergePlugin.hpp"
#include "MergeDefinitions.hpp"

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

namespace tuttle {
namespace plugin {
namespace merge {

mDeclarePluginFactory( MergePluginFactory, { }, { } );

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
using namespace OFX;
void
MergePluginFactory::describe( OFX::ImageEffectDescriptor &desc )
{
    // basic labels
    desc.setLabels( "Merge", "Merge",
                    "Clip merge" );
    desc.setPluginGrouping( "tuttle" );

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth( eBitDepthUByte );
    desc.addSupportedBitDepth( eBitDepthUShort );
    desc.addSupportedBitDepth( eBitDepthFloat );

    // set a few flags
    desc.setSingleInstance( false );
    desc.setHostFrameThreading( false );
    desc.setSupportsMultiResolution( false );
    desc.setSupportsTiles( kSupportTiles );
    desc.setTemporalClipAccess( kSupportTemporalClipAccess );
    desc.setRenderTwiceAlways( false );
    desc.setSupportsMultipleClipPARs( false );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void
MergePluginFactory::describeInContext( OFX::ImageEffectDescriptor &desc,
                                       OFX::ContextEnum context )
{
    OFX::ClipDescriptor *srcClipA = desc.defineClip( kMergeSourceA );
    assert(srcClipA);
    srcClipA->addSupportedComponent( ePixelComponentRGBA );
    srcClipA->addSupportedComponent( ePixelComponentAlpha );
    srcClipA->setSupportsTiles( kSupportTiles );
//    srcClipA->setOptional(false);

    OFX::ClipDescriptor *srcClipB = desc.defineClip( kMergeSourceB );
    assert(srcClipB);
    srcClipB->addSupportedComponent( ePixelComponentRGBA );
    srcClipB->addSupportedComponent( ePixelComponentAlpha );
    srcClipB->setSupportsTiles( kSupportTiles );
//    srcClipB->setOptional(false);

    // Create the mandated output clip
    OFX::ClipDescriptor *dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
    assert(dstClip);
    dstClip->addSupportedComponent( ePixelComponentRGBA );
    dstClip->addSupportedComponent( ePixelComponentAlpha );
    dstClip->setSupportsTiles( kSupportTiles );

    // Define some merging function
    OFX::ChoiceParamDescriptor *mergeFunction = desc.defineChoiceParam( kMergeFunction );
    assert(mergeFunction);
    mergeFunction->appendOption( "atop: Ab+B(1-a)" );
    mergeFunction->appendOption( "average: (A+B)/2" );
    mergeFunction->appendOption( "color-burn: darken B towards A" );
    mergeFunction->appendOption( "color dodge: brighten B towards A" );
    mergeFunction->appendOption( "conjoint-over: A+B(1-a)/b, A if a > b" );
    mergeFunction->appendOption( "copy: A" );
    mergeFunction->appendOption( "difference: abs(A-B)" );
    mergeFunction->appendOption( "disjoint-over: A+B(1-a)/b, A+B if a+b < 1" );
    mergeFunction->appendOption( "divide: A/B, 0 if A < 0 and B < 0" );
    mergeFunction->appendOption( "exclusion: A+B-2AB" );
    mergeFunction->appendOption( "from: B-A" );
    mergeFunction->appendOption( "geometric: 2AB/(A+B)" );
    mergeFunction->appendOption( "hard-light: multiply if A < 0.5, screen if A > 0.5" );
    mergeFunction->appendOption( "hypot: sqrt(A*A+B*B)" );
    mergeFunction->appendOption( "in: Ab" );
    mergeFunction->appendOption( "mask: Ba" );
    mergeFunction->appendOption( "matte: Aa + B(1-a) (unpremultiplied over)" );
    mergeFunction->appendOption( "max: max(A, B)" );
    mergeFunction->appendOption( "min: min(A, B)" );
    mergeFunction->appendOption( "minus: A-B" );
    mergeFunction->appendOption( "multiply: AB, 0 if A < 0 and B < 0" );
    mergeFunction->appendOption( "out: A(1-b)" );
    mergeFunction->appendOption( "over: A+B(1-a)" );
    mergeFunction->appendOption( "overlay: multiply if B<0.5, screen if B>0.5" );
    mergeFunction->appendOption( "plus: A+B" );
    mergeFunction->appendOption( "screen: A+B-AB" );
    mergeFunction->appendOption( "stencil: B(1-a)" );
    mergeFunction->appendOption( "under: A(1-b)+B" );
    mergeFunction->appendOption( "xor: A(1-b)+B(1-a)" );
    mergeFunction->setDefault( eMergeFunctionPlus );

    OFX::PushButtonParamDescriptor *helpButton = desc.definePushButtonParam( kMergeHelpButton );
    helpButton->setScriptName( "&Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect*
MergePluginFactory::createInstance(OfxImageEffectHandle handle,
                                            OFX::ContextEnum context)
{
    return new MergePlugin(handle);
}

}
}
}

namespace OFX 
{
    namespace Plugin 
    {
        void getPluginIDs(OFX::PluginFactoryArray &ids)
        {
            static tuttle::plugin::merge::MergePluginFactory p("fr.hd3d.tuttle.merge", 1, 0);
            ids.push_back(&p);
        }
    }
}
