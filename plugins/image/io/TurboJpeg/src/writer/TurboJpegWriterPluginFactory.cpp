#include "TurboJpegWriterPluginFactory.hpp"
#include "TurboJpegWriterPlugin.hpp"
#include "TurboJpegWriterDefinitions.hpp"

#include <tuttle/ioplugin/context/WriterPluginFactory.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/assign/std/vector.hpp>

#include <string>
#include <vector>

namespace tuttle {
namespace plugin {
namespace turboJpeg {
namespace writer {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void TurboJpegWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels(
		"TuttleTurboJpegWriter",
		"TurboJpegWriter",
		"Turbo Jpeg file writer" );
	desc.setPluginGrouping( "tuttle/image/io" );

	using namespace boost::assign;
	std::vector<std::string> supportedExtensions;
	supportedExtensions += "jpeg", "jpg", "jpe", "jfif", "jfi";
	
	desc.setDescription( "Turbo Jpeg File writer\n"
			 "Plugin is used to write jpeg files.\n\n"
			 "supported extensions: \n" +
			 boost::algorithm::join( supportedExtensions, ", " )
	);

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextWriter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	desc.addSupportedExtensions( supportedExtensions );
	desc.setPluginEvaluation( 90 );
	
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
void TurboJpegWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );
	
	// Controls
	describeWriterParamsInContext( desc, context );
	
	OFX::ChoiceParamDescriptor* channel = static_cast<OFX::ChoiceParamDescriptor*>( desc.getParamDescriptor( kTuttlePluginChannel ) );
	channel->resetOptions();
	channel->appendOption( kTuttlePluginChannelRGB );
	channel->setDefault( 0 );
	channel->setEnabled( false );
	
	OFX::ChoiceParamDescriptor* bitDepth = static_cast<OFX::ChoiceParamDescriptor*>( desc.getParamDescriptor( kTuttlePluginBitDepth ) );
	bitDepth->resetOptions();
	bitDepth->appendOption( kTuttlePluginBitDepth8 );
	bitDepth->setDefault( eTuttlePluginBitDepth8 );
	bitDepth->setEnabled( false );
	
	OFX::BooleanParamDescriptor* premult = static_cast<OFX::BooleanParamDescriptor*>( desc.getParamDescriptor( kParamPremultiplied ) );
	premult->setDefault( true );
	
	OFX::IntParamDescriptor* quality = desc.defineIntParam( kParamQuality );
	quality->setLabel( "Quality" );
	quality->setRange( 0, 100 );
	quality->setDisplayRange( 0, 100 );
	quality->setDefault( 80 );

	
	OFX::ChoiceParamDescriptor* subsampling = desc.defineChoiceParam( kParamSubsampling );
	subsampling->setLabel( kParamSubsamplingLabel );
	subsampling->setHint( kParamSubsamplingHint );
	subsampling->appendOption( kTurboJpegSubsampling444 );
	subsampling->appendOption( kTurboJpegSubsampling422 );
	subsampling->appendOption( kTurboJpegSubsampling420 );
	subsampling->appendOption( kTurboJpegSubsamplingGray );
	subsampling->appendOption( kTurboJpegSubsampling440 );
	subsampling->setDefault( eTurboJpegSubsampling420 );
	
	OFX::ChoiceParamDescriptor* optimization = desc.defineChoiceParam( kParamOptimization );
	optimization->setLabel( kParamOptimizationLabel );
	optimization->setHint( kParamOptimizationHint );
	optimization->appendOption( kTurboJpegOptimizationNone );
	optimization->appendOption( kTurboJpegOptimizationMMX );
	optimization->appendOption( kTurboJpegOptimizationSSE );
	optimization->appendOption( kTurboJpegOptimizationSSE2 );
	optimization->appendOption( kTurboJpegOptimizationSSE3 );
	optimization->setDefault( eTurboJpegOptimizationSSE3 );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* TurboJpegWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new TurboJpegWriterPlugin( handle );
}

}
}
}
}
