#include "OCIOLutPluginFactory.hpp"
#include "OCIOLutPlugin.hpp"
#include "OCIOLutDefinitions.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {
namespace ocio{
namespace lut {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void OCIOLutPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleOcioLut", "OCIOLut", "Color transformation through LUT file using OpenColorIO Transform" );
	desc.setPluginGrouping( "tuttle/image/process/color/ocio" );

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextGeneral );
	desc.addSupportedContext( OFX::eContextFilter );

	// add supported pixel depths
//	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
//	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	desc.setSupportsTiles( kSupportTiles );
	//desc.setRenderThreadSafety( OFX::eRenderFullySafe ); //< @todo tuttle: remove process data from LutPlugin
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void OCIOLutPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc, OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->setSupportsTiles( kSupportTiles );

	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->setSupportsTiles( kSupportTiles );

	// Controls
	OFX::StringParamDescriptor* filename = desc.defineStringParam( kParamInputFilename );
	filename->setDefault( "" );
	filename->setLabels( kParamInputFilenameLabel, kParamInputFilenameLabel, kParamInputFilenameLabel );
	filename->setStringType( OFX::eStringTypeFilePath );

	OFX::ChoiceParamDescriptor* interpolationType = desc.defineChoiceParam( kParamInterpolationType );
	filename->setLabels( kParamInterpolationTypeLabel, kParamInterpolationTypeLabel, kParamInterpolationTypeLabel );
	interpolationType->appendOption( kParamInterpolationNearest );
	interpolationType->appendOption( kParamInterpolationLinear );
	interpolationType->appendOption( kParamInterpolationTetrahedral );
	interpolationType->setDefault(eInterpolationTypeLinear);


}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* OCIOLutPluginFactory::createInstance( OfxImageEffectHandle handle, OFX::EContext context )
{
	return new OCIOLutPlugin( handle );
}

}
}
}
}
