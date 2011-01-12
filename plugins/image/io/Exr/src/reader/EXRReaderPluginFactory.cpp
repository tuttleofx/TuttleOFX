#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPluginFactory.hpp"
#include "EXRReaderPlugin.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void EXRReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleExrReader", "ExrReader",
	                "Exr file reader" );
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
void EXRReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                OFX::EContext               context )
{
	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );

	// Exr only supports RGB(A)
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->setSupportsTiles( kSupportTiles );

	// Controls
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kParamReaderFilename );
	assert( filename );
	filename->setLabel( "Filename" );
	filename->setStringType( OFX::eStringTypeFilePath );
	filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	desc.addClipPreferencesSlaveParam( *filename );

	OFX::ChoiceParamDescriptor* explicitConversion = desc.defineChoiceParam( kParamReaderExplicitConversion );
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

	OFX::ChoiceParamDescriptor* outComponents = desc.defineChoiceParam( kParamOutputComponents );
	assert( outComponents );
	outComponents->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	outComponents->setEvaluateOnChange( true );
	outComponents->setLabel( "Components to output" );
	outComponents->setDefault( 0 );

	OFX::ChoiceParamDescriptor* outRedIs = desc.defineChoiceParam( kParamOutputRedIs );
	assert( outRedIs );
	outRedIs->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	outRedIs->setEvaluateOnChange( true );
	outRedIs->setLabel( "Red is" );
	outRedIs->setDefault( 0 );

	OFX::ChoiceParamDescriptor* outGreenIs = desc.defineChoiceParam( kParamOutputGreenIs );
	assert( outGreenIs );
	outGreenIs->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	outGreenIs->setEvaluateOnChange( true );
	outGreenIs->setLabel( "Green is" );
	outGreenIs->setDefault( 0 );

	OFX::ChoiceParamDescriptor* outBlueIs = desc.defineChoiceParam( kParamOutputBlueIs );
	assert( outBlueIs );
	outBlueIs->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	outBlueIs->setEvaluateOnChange( true );
	outBlueIs->setLabel( "Blue is" );
	outBlueIs->setDefault( 0 );

	OFX::ChoiceParamDescriptor* outAlphaIs = desc.defineChoiceParam( kParamOutputAlphaIs );
	assert( outAlphaIs );
	outAlphaIs->setEvaluateOnChange( true );
	outAlphaIs->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	outAlphaIs->setLabel( "Alpha is" );
	outAlphaIs->setDefault( 0 );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* EXRReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                          OFX::EContext        context )
{
	return new EXRReaderPlugin( handle );
}

}
}
}
}
