#include "DPXWriterPluginFactory.hpp"
#include "DPXWriterPlugin.hpp"
#include "DPXWriterDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
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
void DPXWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleDpxWriter", "DpxWriter",
	                "Dpx file writer" );
	desc.setPluginGrouping( "tuttle" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextWriter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsMultiResolution( false );
	desc.setSupportsTiles( kSupportTiles );
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
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	// Dpx only supports RGB(A)
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->setSupportsTiles( kSupportTiles );

	// Controls
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kOutputFilename );
	filename->setLabels( kOutputFilenameLabel, kOutputFilenameLabel, kOutputFilenameLabel );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );

	OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kParamBitDepth );
	bitDepth->setLabels( kParamBitDepthLabel, kParamBitDepthLabel, kParamBitDepthLabel );
	bitDepth->appendOption( "16 bits output" );
	bitDepth->appendOption( "12 bits output" );
	bitDepth->appendOption( "10 bits output" );
	bitDepth->appendOption( "8 bits output" );
	bitDepth->setDefault( 3 );

	OFX::ChoiceParamDescriptor* componentsType = desc.defineChoiceParam( kParamComponentsType );
	componentsType->setLabels( kParamComponentsTypeLabel, kParamComponentsTypeLabel, kParamComponentsTypeLabel );
	componentsType->appendOption( "rgb  output" );
	componentsType->appendOption( "rgba output" );
	componentsType->appendOption( "abgr output" );
	componentsType->setDefault( 1 );

	OFX::BooleanParamDescriptor* compressed = desc.defineBooleanParam( kParamCompressed );
	compressed->setLabels( kParamCompressedLabel, kParamCompressedLabel, "Remove unused bits (bit streaming)" );
	compressed->setDefault( false );

	OFX::PushButtonParamDescriptor* renderButton = desc.definePushButtonParam( kRender );
	renderButton->setLabels( kRenderLabel, kRenderLabel, "Step rendering" );
	renderButton->setHint( "Avoid to render the same image" );
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
