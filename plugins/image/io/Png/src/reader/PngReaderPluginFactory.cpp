#include "PngReaderPluginFactory.hpp"
#include "PngReaderDefinitions.hpp"
#include "PngReaderPlugin.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {
namespace png {
namespace reader {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void PngReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttlePngReader", "PngReader",
	                "Png file reader" );
	desc.setPluginGrouping( "tuttle/image/io" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextReader );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
	desc.setHostFrameThreading( false );
	desc.setSupportsMultiResolution( false );
	desc.setSupportsMultipleClipDepths( true );
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void PngReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                OFX::EContext               context )
{
	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );

	assert( dstClip );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	// Controls
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kReaderParamFilename );
	assert( filename );
	filename->setLabel( "Filename" );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	desc.addClipPreferencesSlaveParam( *filename );

	OFX::ChoiceParamDescriptor* explicitConversion = desc.defineChoiceParam( kReaderParamExplicitConversion );
	explicitConversion->setLabel( "Explicit conversion" );
	explicitConversion->appendOption( kTuttlePluginBitDepthAuto );
	explicitConversion->appendOption( kTuttlePluginBitDepth8 );
	explicitConversion->appendOption( kTuttlePluginBitDepth16 );
	explicitConversion->appendOption( kTuttlePluginBitDepth32f );
	explicitConversion->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	explicitConversion->setAnimates( false );
	desc.addClipPreferencesSlaveParam( *explicitConversion );

	if( OFX::getImageEffectHostDescription()->supportsMultipleClipDepths )
	{
		explicitConversion->setDefault( 0 );
	}
	else
	{
		explicitConversion->setIsSecret( true );
		explicitConversion->setDefault( static_cast<int>( OFX::getImageEffectHostDescription()->getPixelDepth() ) );
	}
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* PngReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                          OFX::EContext        context )
{
	return new PngReaderPlugin( handle );
}

}
}
}
}
