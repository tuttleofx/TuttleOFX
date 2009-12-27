/**
 * @file DPXWriterPluginFactory.cpp
 * @brief
 * @author
 * @date    16/12/09 15:34
 *
 */

#include "DPXWriterPluginFactory.hpp"
#include "DPXWriterPlugin.hpp"
#include "DPXWriterDefinitions.hpp"

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
namespace dpx {
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
using namespace OFX;
void DPXWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	// basic labels
	desc.setLabels( "DPXWriterHd3d", "DPXWriterHd3d",
	                "DPX File writer" );
	desc.setPluginGrouping( "tuttle" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( eContextGeneral );

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
void DPXWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                OFX::ContextEnum            context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );

	// Dpx only supports RGB(A)
	srcClip->addSupportedComponent( ePixelComponentRGBA );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	// Dpx only supports RGB(A)
	dstClip->addSupportedComponent( ePixelComponentRGBA );
	dstClip->setSupportsTiles( kSupportTiles );

	// Controls
	StringParamDescriptor* filename = desc.defineStringParam( kOutputFilename );
	filename->setScriptName( "Output filename" );
	filename->setStringType( eStringTypeFilePath );
	filename->setCacheInvalidation( eCacheInvalidateValueAll );

	OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kParamBitDepth );
	bitDepth->setScriptName( "bitDepth" );
	bitDepth->appendOption( "16 bits output" );
	bitDepth->appendOption( "12 bits output" );
	bitDepth->appendOption( "10 bits output" );
	bitDepth->appendOption( "8 bits output" );
	bitDepth->setDefault( 0 );

	OFX::ChoiceParamDescriptor* componentsType = desc.defineChoiceParam( kParamComponentsType );
	componentsType->setScriptName( "componentsType" );
	componentsType->appendOption( "rgb  output" );
	componentsType->appendOption( "rgba output" );
	componentsType->appendOption( "abgr output" );
	componentsType->setDefault( 1 );

	OFX::BooleanParamDescriptor* compressed = desc.defineBooleanParam( kParamCompressed );
	compressed->setLabels( "Compressed", "Compressed", "Remove unused bits (bit streaming)" );
	compressed->setDefault( false );

	PushButtonParamDescriptor* renderButton = desc.definePushButtonParam( kRender );
	renderButton->setScriptName( "renderButton" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* DPXWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                          OFX::ContextEnum     context )
{
	return new DPXWriterPlugin( handle );
}

}
}
}
}
