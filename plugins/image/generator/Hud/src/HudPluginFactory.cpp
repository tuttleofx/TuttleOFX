#include "HudPluginFactory.hpp"
#include "HudPlugin.hpp"
#include "HudDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle {
namespace plugin {
namespace hud {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void HudPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels(
		"Hud",
		"Hud",
		"Hud" );
	desc.setPluginGrouping( "tuttle/image/generator" );

	desc.setDescription( "Plugin under early development." );

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
void HudPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
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

	OFX::RGBAParamDescriptor* color1 = desc.defineRGBAParam( kConstantColor );
	color1->setDefault( 0, 0, 0, 1 );
	color1->setLabel( "Color" );

	// Define some merging function
	OFX::ChoiceParamDescriptor* mergeFunction = desc.defineChoiceParam( kParamFunction );
	mergeFunction->setLabels( kParamFunctionLabel, kParamFunctionLabel, kParamFunctionLabel );
	mergeFunction->appendOption( "atop", "atop: Ab+B(1-a)" );
	mergeFunction->appendOption( "average", "average: (A+B)/2" );
	mergeFunction->appendOption( "color", "color: hue from B, saturation from B, lightness from A" );
	mergeFunction->appendOption( "color-burn", "color-burn: darken B towards A" );
	mergeFunction->appendOption( "color dodge inversed", "color dodge inversed: brighten B towards A" );
	mergeFunction->appendOption( "conjoint-over", "conjoint-over: A+B(1-a)/b, A if a > b" );
	mergeFunction->appendOption( "copy", "copy: A" );
	mergeFunction->appendOption( "difference", "difference: abs(A-B)" );
	mergeFunction->appendOption( "disjoint-over", "disjoint-over: A+B(1-a)/b, A+B if a+b < 1" );
	mergeFunction->appendOption( "divide", "divide: A/B, 0 if A < 0 and B < 0" );
	mergeFunction->appendOption( "exclusion", "exclusion: A+B-2AB" );
	mergeFunction->appendOption( "freeze", "freeze: 1-sqrt(1-A)/B" );
	mergeFunction->appendOption( "from", "from: B-A" );
	mergeFunction->appendOption( "geometric", "geometric: 2AB/(A+B)" );
	mergeFunction->appendOption( "hard-light", "hard-light: multiply if A < 0.5, screen if A > 0.5" );
	mergeFunction->appendOption( "hypot", "hypot: sqrt(A*A+B*B)" );
	mergeFunction->appendOption( "in", "in: Ab" );
	mergeFunction->appendOption( "interpolated", "interpolated: (like average but better and slower)" );
	mergeFunction->appendOption( "mask", "mask: Ba" );
	mergeFunction->appendOption( "matte", "matte: Aa + B(1-a) (unpremultiplied over)" );
	mergeFunction->appendOption( "lighten", "lighten: max(A, B)" );
	mergeFunction->appendOption( "darken", "darken: min(A, B)" );
	mergeFunction->appendOption( "minus", "minus: A-B" );
	mergeFunction->appendOption( "multiply", "multiply: AB, 0 if A < 0 and B < 0" );
	mergeFunction->appendOption( "out", "out: A(1-b)" );
	mergeFunction->appendOption( "over", "over: A+B(1-a)" );
	mergeFunction->appendOption( "overlay", "overlay: multiply if B<0.5, screen if B>0.5" );
	mergeFunction->appendOption( "pinlight", "pinlight: if B >= 0.5 then max(A, 2*B - 1), min(A, B * 2.0 ) else" );
	mergeFunction->appendOption( "plus", "plus: A+B" );
	mergeFunction->appendOption( "reflect", "reflect: a² / (1 - b)" );
	mergeFunction->appendOption( "screen", "screen: A+B-AB" );
	mergeFunction->appendOption( "stencil", "stencil: B(1-a)" );
	mergeFunction->appendOption( "under", "under: A(1-b)+B" );
	mergeFunction->appendOption( "xor", "xor: A(1-b)+B(1-a)" );
	mergeFunction->setDefault( eParamMergePlus );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* HudPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new HudPlugin( handle );
}

}
}
}

