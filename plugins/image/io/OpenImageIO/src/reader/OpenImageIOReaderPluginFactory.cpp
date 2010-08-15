#include "OpenImageIOReaderPluginFactory.hpp"
#include "OpenImageIOReaderDefinitions.hpp"
#include "OpenImageIOReaderPlugin.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace reader {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void OpenImageIOReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleOpenImageIOReader", "OpenImageIOReader",
	                "OpenImageIO file reader" );
	desc.setPluginGrouping( "tuttle/image/io" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextReader );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );

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
void OpenImageIOReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                OFX::ContextEnum            context )
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
OFX::ImageEffect* OpenImageIOReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                          OFX::ContextEnum     context )
{
	return new OpenImageIOReaderPlugin( handle );
}

}
}
}
}
