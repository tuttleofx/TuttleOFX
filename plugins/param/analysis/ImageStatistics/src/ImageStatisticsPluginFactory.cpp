#include "ImageStatisticsPlugin.hpp"
#include "ImageStatisticsDefinitions.hpp"
#include "ImageStatisticsOverlayInteract.hpp"

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
namespace imageStatistics {

static const bool kSupportTiles = false;

mDeclarePluginFactory( ImageStatisticsPluginFactory, { }, { } );

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void ImageStatisticsPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleImageStatistics", "ImageStatistics",
		            "Image statistics" );
	desc.setPluginGrouping( "tuttle" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextGeneral );
	desc.addSupportedContext( OFX::eContextFilter );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsMultiResolution( false );
	desc.setSupportsTiles( kSupportTiles );
	
    desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayWrap<ImageStatisticsEffectOverlayDescriptor>() );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ImageStatisticsPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::ContextEnum context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	OFX::Double2DParamDescriptor* rectCenter = desc.defineDouble2DParam( kRectCenter );
	rectCenter->setLabel( "Center" );
	rectCenter->setDoubleType( OFX::eDoubleTypePlain );
//	rectCenter->setDoubleType( OFX::eDoubleTypeNormalisedXYAbsolute );
    rectCenter->setDefault( 0.0, 0.0 );

	OFX::Double2DParamDescriptor* rectSize = desc.defineDouble2DParam( kRectSize );
	rectSize->setLabel( "Size" );
	rectSize->setDoubleType( OFX::eDoubleTypePlain );
//	rectSize->setDoubleType( OFX::eDoubleTypeNormalisedXYAbsolute );
    rectSize->setDefault( 100, 100 );

	OFX::ChoiceParamDescriptor* chooseOutput = desc.defineChoiceParam( kChooseOutput );
	chooseOutput->setLabel( "Choose output" );
	chooseOutput->appendOption( kChooseOutputSource );
	chooseOutput->appendOption( kChooseOutputAverage );
	chooseOutput->appendOption( kChooseOutputChannelMin );
	chooseOutput->appendOption( kChooseOutputChannelMax );
	chooseOutput->appendOption( kChooseOutputLuminosityMin );
	chooseOutput->appendOption( kChooseOutputLuminosityMax );
	chooseOutput->setDefault( 0 );

	OFX::GroupParamDescriptor* outputGroup = desc.defineGroupParam( kOutputGroup );
	outputGroup->setLabel( "Output" );

	OFX::RGBAParamDescriptor* outputAverage = desc.defineRGBAParam( kOutputAverage );
	outputAverage->setLabel( "Average" );
	outputAverage->setParent( outputGroup );
	outputAverage->setEvaluateOnChange( false );

	OFX::Double3DParamDescriptor* outputAverageHsl = desc.defineDouble3DParam( kOutputAverageHsl );
	outputAverageHsl->setLabel( "Average HSL" );
	outputAverageHsl->setDoubleType( OFX::eDoubleTypePlain );
	outputAverageHsl->setDimensionLabels( "h", "s", "l" );
	outputAverageHsl->setParent( outputGroup );
	outputAverageHsl->setEvaluateOnChange( false );

	OFX::RGBAParamDescriptor* outputChannelMin = desc.defineRGBAParam( kOutputChannelMin );
	outputChannelMin->setLabel( "Channels' min" );
	outputChannelMin->setHint( "Minimum value per channel" );
	outputChannelMin->setParent( outputGroup );
	outputChannelMin->setEvaluateOnChange( false );

	OFX::RGBAParamDescriptor* outputChannelMax = desc.defineRGBAParam( kOutputChannelMax );
	outputChannelMax->setLabel( "Channels' max" );
	outputChannelMax->setParent( outputGroup );
	outputChannelMax->setEvaluateOnChange( false );

	OFX::RGBAParamDescriptor* outputLuminosityMin = desc.defineRGBAParam( kOutputLuminosityMin );
	outputLuminosityMin->setLabel( "Luminosity min" );
	outputLuminosityMin->setParent( outputGroup );
	outputLuminosityMin->setEvaluateOnChange( false );

	OFX::RGBAParamDescriptor* outputLuminosityMax = desc.defineRGBAParam( kOutputLuminosityMax );
	outputLuminosityMax->setLabel( "Luminosity max" );
	outputLuminosityMax->setParent( outputGroup );
	outputLuminosityMax->setEvaluateOnChange( false );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ImageStatisticsPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::ContextEnum context )
{
	return new ImageStatisticsPlugin(handle);
}

}
}
}

namespace OFX {
namespace Plugin {

void getPluginIDs( OFX::PluginFactoryArray& ids )
{
	static tuttle::plugin::imageStatistics::ImageStatisticsPluginFactory p("fr.tuttle.imagestatistics", 1, 0);
	ids.push_back(&p);
}

}
}
