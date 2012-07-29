#include "SwscalePluginFactory.hpp"
#include "SwscalePlugin.hpp"
#include "SwscaleDefinitions.hpp"

#include <tuttle/plugin/context/SamplerPluginFactory.hpp>

#include <limits>


namespace tuttle {
namespace plugin {
namespace swscale {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void SwscalePluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleSwscale", "Swscale", "Swscale" );
	desc.setPluginGrouping( "tuttle/image/process/geometry" );
	
	desc.setDescription( "SwScale: fast resizing plugin\n"
				"Plugin using swscale library from FFMpeg.\n"
				"Warning: Could not run with floating point images and\n"
				"			RGBA data different of 8 bit (32bpp).\n"
	);

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	//desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void SwscalePluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc, OFX::EContext context )
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

	OFX::ChoiceParamDescriptor* method = desc.defineChoiceParam( kParamMode );
	method->setLabel( "Mode" );
	method->appendOption( kParamModeFormat );
	method->appendOption( kParamModeSize );
	method->appendOption( kParamModeScale );
	method->setDefault( eParamModeFormat );

	OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamFormat );
	format->setLabel( "Format" );
	format->appendOption( kParamFormatPCVideo, kParamFormatPCVideoLabel );
	format->appendOption( kParamFormatNTSC, kParamFormatNTSCLabel );
	format->appendOption( kParamFormatPAL, kParamFormatPALLabel );
	format->appendOption( kParamFormatHD, kParamFormatHDLabel );
	format->appendOption( kParamFormatNTSC169, kParamFormatNTSC169Label );
	format->appendOption( kParamFormatPAL169, kParamFormatPAL169Label );
	format->appendOption( kParamFormat1kSuper35, kParamFormat1kSuper35Label );
	format->appendOption( kParamFormat1kCinemascope, kParamFormat1kCinemascopeLabel );
	format->appendOption( kParamFormat2kSuper35, kParamFormat2kSuper35Label );
	format->appendOption( kParamFormat2kCinemascope, kParamFormat2kCinemascopeLabel );
	format->appendOption( kParamFormat4kSuper35, kParamFormat4kSuper35Label );
	format->appendOption( kParamFormat4kCinemascope, kParamFormat4kCinemascopeLabel );
	format->appendOption( kParamFormatSquare256, kParamFormatSquare256Label );
	format->appendOption( kParamFormatSquare512, kParamFormatSquare512Label );
	format->appendOption( kParamFormatSquare1k, kParamFormatSquare1kLabel );
	format->appendOption( kParamFormatSquare2k, kParamFormatSquare2kLabel );
	format->setDefault( eParamFormat2kCinemascope );

	OFX::Double2DParamDescriptor* scale = desc.defineDouble2DParam( kParamScale );
	scale->setLabel( "Scale" );
	scale->setDefault( 1.0, 1.0 );
	scale->setRange( 0.01, 0.01, std::numeric_limits<double>::max(), std::numeric_limits<double>::max() );
	scale->setDisplayRange( 0.1, 0.1, 2.5, 2.5 );
	scale->setHint( "Scale the input image [0, 0, width*scale, height*scale]." );

	OFX::BooleanParamDescriptor* keepRatio = desc.defineBooleanParam( kParamSizeKeepRatio );
	keepRatio->setLabel( "Keep ratio" );
	keepRatio->setDefault( false );
	keepRatio->setHint( "Keep input image ratio." );

	OFX::Int2DParamDescriptor* size = desc.defineInt2DParam( kParamSize );
	size->setLabel( "Size" );
	size->setDefault( 200, 200 );
	size->setRange( 1, 1, std::numeric_limits<int>::max(), std::numeric_limits<int>::max() );
	size->setHint( "Set the output size (width, height)." );

	OFX::ChoiceParamDescriptor* direction = desc.defineChoiceParam( kParamSizeOrientation );
	direction->setLabel( "Orientation" );
	direction->appendOption( kParamSizeOrientationX );
	direction->appendOption( kParamSizeOrientationY );
	direction->setDefault( eParamSizeOrientationX );

	OFX::IntParamDescriptor* width = desc.defineIntParam( kParamSizeWidth );
	width->setLabel( "Width" );
	width->setDefault( 200 );
	width->setRange( 1, std::numeric_limits<int>::max() );
	width->setDisplayRange( 0, 3000 );
	width->setHint( "Set the width in pixels and keep the input image ratio." );

	OFX::IntParamDescriptor* height = desc.defineIntParam( kParamSizeHeight );
	height->setLabel( "Height" );
	height->setDefault( 200 );
	height->setRange( 1, std::numeric_limits<int>::max() );
	height->setDisplayRange( 0, 3000 );
	height->setHint( "Set the height in pixels and keep the input image ratio." );

	// sampler parameters //
	OFX::ChoiceParamDescriptor* filter = desc.defineChoiceParam( kParamFilter );
	filter->setLabel( "Filter" );
	filter->appendOption( kParamFilterFastBilinear );
	filter->appendOption( kParamFilterBilinear );
	filter->appendOption( kParamFilterBicubic );
	filter->appendOption( kParamFilterX );
	filter->appendOption( kParamFilterPoint );
	filter->appendOption( kParamFilterArea );
	filter->appendOption( kParamFilterBicublin );
	filter->appendOption( kParamFilterGauss );
	filter->appendOption( kParamFilterSinc );
	filter->appendOption( kParamFilterLanczos );
	filter->appendOption( kParamFilterSpline );
	filter->setDefault( eParamFilterBicubic );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* SwscalePluginFactory::createInstance( OfxImageEffectHandle handle,
														OFX::EContext context )
{
	return new SwscalePlugin( handle );
}

}
}
}

