#include "CannyPluginFactory.hpp"
#include "CannyPlugin.hpp"
#include "CannyDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace canny {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void CannyPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "Canny", "Canny",
		            "Canny" );
	desc.setPluginGrouping( "tuttle" );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void CannyPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::EContext context )
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

//	min
//	max
//	OFX::Double2DParamDescriptor* size = desc.defineDouble2DParam( kParamSize );
//	size->setLabel( "Size" );
//	size->setDefault( 3, 3 );
//	size->setRange( 0.0, 0.0, std::numeric_limits<double>::max(), std::numeric_limits<double>::max() );
//	size->setDisplayRange( 0, 0, 10, 10 );
//	size->setDoubleType( OFX::eDoubleTypeScale );

	OFX::BooleanParamDescriptor* nonMaximaSuppression = desc.defineBooleanParam( kParamNonMaximaSuppression );
	nonMaximaSuppression->setLabel( "Non-maxima suppression" );
	nonMaximaSuppression->setDefault( true );
	
	OFX::BooleanParamDescriptor* hysteresisThresholding = desc.defineBooleanParam( kParamHysteresisThresholding );
	hysteresisThresholding->setLabel( "Hysteresis thresholding" );
	hysteresisThresholding->setDefault( true );

	OFX::BooleanParamDescriptor* thinning = desc.defineBooleanParam( kParamThinning );
	thinning->setLabel( "Thinning" );
	thinning->setDefault( true );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* CannyPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new CannyPlugin( handle );
}

}
}
}

