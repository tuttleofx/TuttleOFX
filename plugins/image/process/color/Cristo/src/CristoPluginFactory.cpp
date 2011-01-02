#include "CristoPluginFactory.hpp"
#include "CristoPlugin.hpp"
#include "CristoDefinitions.hpp"

#include <limits>

namespace tuttle {
namespace plugin {
namespace cristo {

static const bool kSupportTiles = true;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void CristoPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "Cristo", "Cristo",
		            "Cristo" );
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
	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void CristoPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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

	OFX::ParametricParamDescriptor* curves = desc.defineParametricParam( kParamColorSelection );
	curves->setDimension( nbCurves );
	curves->setDimensionLabel( kParamColorSelectionRed, 0 );
	curves->setDimensionLabel( kParamColorSelectionGreen, 1 );
	curves->setDimensionLabel( kParamColorSelectionBlue, 2 );
	curves->setDimensionLabel( kParamColorSelectionHue, 3 );
	curves->setDimensionLabel( kParamColorSelectionSaturation, 4 );
	curves->setDimensionLabel( kParamColorSelectionLightness, 5 );
	curves->setHint( "Color selection" );

	OFX::PushButtonParamDescriptor* helpButton = desc.definePushButtonParam( kParamHelpButton );
	helpButton->setLabel( "Help" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* CristoPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new CristoPlugin( handle );
}

}
}
}

