#include "ColorGradationPluginFactory.hpp"
#include "ColorGradationPlugin.hpp"
#include "ColorGradationDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle {
namespace plugin {
namespace colorGradation {

static const bool kSupportTiles = true;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ColorGradationPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleColorGradation", "ColorGradation",
	                "ColorGradation" );
	desc.setPluginGrouping( "tuttle/image/process/color" );
	desc.setDescription(
"Color distribution / Color gradation"
"\n"
"\n"
"http://en.wikipedia.org/wiki/Gamma_correction"
"\n"
	/// @todo documentation: definition of color gradation
	);
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
void ColorGradationPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                        OFX::EContext               context )
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

	OFX::ChoiceParamDescriptor* in = desc.defineChoiceParam( kParamIn );
	in->setLabel( "In" );
	in->setHint( "Input color gradation." );
	in->appendOption( kParamGradation_linear );
	in->appendOption( kParamGradation_sRGB );
	in->appendOption( kParamGradation_cineon );
	in->appendOption( kParamGradation_gamma );
	in->appendOption( kParamGradation_panalog );
	in->appendOption( kParamGradation_REDLog );
	in->appendOption( kParamGradation_ViperLog );
	in->appendOption( kParamGradation_REDSpace );
	in->appendOption( kParamGradation_AlexaLogC );

	//	in->appendOption( kParamGradation_rec709 );
	//	in->appendOption( kParamGradation_rec601 );
	in->setDefault( 0 );

	OFX::ChoiceParamDescriptor* out = desc.defineChoiceParam( kParamOut );
	out->setLabel( "Out" );
	out->setHint( "Output color gradation." );
	out->appendOption( kParamGradation_linear );
	out->appendOption( kParamGradation_sRGB );
	out->appendOption( kParamGradation_cineon );
	out->appendOption( kParamGradation_gamma );
	out->appendOption( kParamGradation_panalog );
	out->appendOption( kParamGradation_REDLog );
	out->appendOption( kParamGradation_ViperLog );
	out->appendOption( kParamGradation_REDSpace );
	out->appendOption( kParamGradation_AlexaLogC );

	//	out->appendOption( kParamGradation_rec709 );
	//	out->appendOption( kParamGradation_rec601 );
	out->setDefault( 0 );

	OFX::BooleanParamDescriptor* alpha = desc.defineBooleanParam( kParamProcessAlpha );
	alpha->setLabel( "Process alpha" );
	alpha->setHint( "Apply the conversion on alpha channel." );
	alpha->setDefault( false );

	OFX::PushButtonParamDescriptor* invert = desc.definePushButtonParam( kParamInvert );
	invert->setLabel( "Invert" );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ColorGradationPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                                  OFX::EContext        context )
{
	return new ColorGradationPlugin( handle );
}

}
}
}

