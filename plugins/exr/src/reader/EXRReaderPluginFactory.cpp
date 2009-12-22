/**
 * @author Eloi Du Bois.
 * @date   2009/10/11
 *
 */

#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPluginFactory.hpp"
#include "EXRReaderPlugin.hpp"
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
namespace exr {
namespace reader {

using namespace OFX;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void EXRReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	// basic labels
	desc.setLabels( "EXRReaderHd3d", "EXRReaderHd3d",
	                "EXR File reader Hd3d" );
	desc.setPluginGrouping( "tuttle" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( eContextGenerator );

	// add supported pixel depths
	desc.addSupportedBitDepth( eBitDepthFloat );
	desc.addSupportedBitDepth( eBitDepthUByte );
	desc.addSupportedBitDepth( eBitDepthUShort );

	// set a few flags
	desc.setSingleInstance( false );
	desc.setHostFrameThreading( false );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsTiles( kSupportTiles );
	desc.setTemporalClipAccess( false );
	desc.setRenderTwiceAlways( false );
	desc.setSupportsMultipleClipPARs( false );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void EXRReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                OFX::ContextEnum            context )
{
	// Create the mandated output clip
	ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );

	// Exr only supports RGB(A)
	dstClip->addSupportedComponent( ePixelComponentRGBA );
	dstClip->setSupportsTiles( kSupportTiles );

	// Controls
	StringParamDescriptor* filename = desc.defineStringParam( kInputFilename );
	assert( filename );
	filename->setScriptName( "filename" );
	filename->setStringType( eStringTypeFilePath );
	filename->setCacheInvalidation( eCacheInvalidateValueAll );

	OFX::ChoiceParamDescriptor* outComponents = desc.defineChoiceParam( kOutputComponents );
	assert( outComponents );
	OFX::ChoiceParamDescriptor* outRedIs = desc.defineChoiceParam( kOutputRedIs );
	assert( outRedIs );
	OFX::ChoiceParamDescriptor* outGreenIs = desc.defineChoiceParam( kOutputGreenIs );
	assert( outGreenIs );
	OFX::ChoiceParamDescriptor* outBlueIs = desc.defineChoiceParam( kOutputBlueIs );
	assert( outBlueIs );
	OFX::ChoiceParamDescriptor* outAlphaIs = desc.defineChoiceParam( kOutputAlphaIs );
	assert( outAlphaIs );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* EXRReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                          OFX::ContextEnum     context )
{
	return new EXRReaderPlugin( handle );
}

}
}
}
}
