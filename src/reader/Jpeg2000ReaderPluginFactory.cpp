#include "Jpeg2000ReaderPluginFactory.hpp"
#include "Jpeg2000ReaderPlugin.hpp"
#include "Jpeg2000ReaderDefinitions.hpp"
#include "tuttle/plugin/context/WriterDefinition.hpp"

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
namespace jpeg2000 {
namespace reader {


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void Jpeg2000ReaderPluginFactory::describe( OFX::ImageEffectDescriptor &desc )
{
	desc.setLabels( "DuJpeg2000Reader", "Jpeg2000Reader",
		            "Jpeg2000 image reader" );
	desc.setPluginGrouping( "duplo" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextReader );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsMultipleClipDepths( true );
	desc.setRenderThreadSafety( OFX::eRenderUnsafe );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void Jpeg2000ReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor &desc,
                                                   OFX::ContextEnum context )
{
	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	assert( dstClip );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	// Controls
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kTuttlePluginReaderParamFilename );
	assert( filename );
	filename->setLabel( "Filename" );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	desc.addClipPreferencesSlaveParam( *filename );

	OFX::ChoiceParamDescriptor* explicitConversion = desc.defineChoiceParam( kTuttlePluginReaderParamExplicitConversion );
	explicitConversion->setLabel( "Explicit conversion" );
	explicitConversion->appendOption( kTuttlePluginBitDepthAuto );
	explicitConversion->appendOption( kTuttlePluginBitDepth8 );
	explicitConversion->appendOption( kTuttlePluginBitDepth16 );
	explicitConversion->appendOption( kTuttlePluginBitDepth32f );
	desc.addClipPreferencesSlaveParam( *explicitConversion );

	if( ! OFX::getImageEffectHostDescription()->supportsMultipleClipDepths )
	{
		explicitConversion->setIsSecret( true );
		if( OFX::getImageEffectHostDescription()->_supportedPixelDepths.size() == 1 )
		{
			explicitConversion->setDefault( static_cast<int>(OFX::getImageEffectHostDescription()->_supportedPixelDepths[0]) );
		}
		else
		{
			COUT_WARNING("The host doesn't support multiple clip depths, but didn't define supported pixel depth. (size: " << OFX::getImageEffectHostDescription()->_supportedPixelDepths.size() << ")" );
			explicitConversion->setDefault( 3 );
		}
	}
	else
	{
		explicitConversion->setDefault( 0 );
	}
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* Jpeg2000ReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::ContextEnum context )
{
	return new Jpeg2000ReaderPlugin(handle);
}

}
}
}
}
