#include "ColorSpaceKeyerPluginFactory.hpp"
#include "ColorSpaceKeyerPlugin.hpp"
#include "ColorSpaceKeyerDefinitions.hpp"
#include "ColorSpaceKeyerOverlay.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle {
namespace plugin {
namespace colorSpaceKeyer {

static const bool kSupportTiles = false;


/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ColorSpaceKeyerPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	//describe the plugin
	desc.setLabels( "TuttleColorSpaceKeyer", "ColorSpaceKeyer",
		            "ColorSpaceKeyer" );
	desc.setPluginGrouping( "tuttle/image/process/color" );

	desc.setDescription( "ColorSpaceKeyer\n"
	                     "Plugin is used to ???." );

	// add the supported contexts, only filter at the moment
	desc.addSupportedContext( OFX::eContextFilter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles( kSupportTiles );
//	desc.setRenderThreadSafety( OFX::eRenderFullySafe );
	desc.setRenderThreadSafety( OFX::eRenderUnsafe );
	
	//Overlay class
	desc.setOverlayInteractDescriptor( new OFX::DefaultEffectOverlayWrap<ColorSpaceKeyerOverlayDescriptor>() );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ColorSpaceKeyerPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                  OFX::EContext context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
	srcClip->setSupportsTiles( kSupportTiles );

	OFX::ClipDescriptor* strongSelectionClip = desc.defineClip( kClipColorSelection );
	strongSelectionClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	strongSelectionClip->setOptional( true );
	strongSelectionClip->setSupportsTiles( kSupportTiles );
	
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->setSupportsTiles( kSupportTiles );

	//Check-box display
	OFX::BooleanParamDescriptor* pointCloudDisplay = desc.defineBooleanParam( kPointCloudDisplay );
	pointCloudDisplay->setLabel( kPointCloudDisplayLabel );		//label
	pointCloudDisplay->setDefault(false);						//display overlay by default
	
	//Check-box discretization cloud point display
	OFX::BooleanParamDescriptor* pointCloudDiscretization = desc.defineBooleanParam( kBoolDiscretizationDisplay );
	pointCloudDiscretization->setLabel( kBoolDiscretizationDisplayLabel );		//label
	pointCloudDiscretization->setDefault(false);								//display overlay by default
	pointCloudDiscretization->setLayoutHint( OFX::eLayoutHintNoNewLine );		//line is not finished
	pointCloudDiscretization->setEnabled(false);								//Disabled by default (display cloud point is not selected)
	pointCloudDiscretization->setHint("Activate discretization point cloud.");
	
	//Discretization int range
	OFX::IntParamDescriptor* discretizationDisplay = desc.defineIntParam(kIntDiscretizationDisplay);
	discretizationDisplay->setLabel(kIntDiscretizationDisplayLabel);			//label
	discretizationDisplay->setRange(1,500);										//value range
	discretizationDisplay->setDisplayRange(1,30);								//display value range
	discretizationDisplay->setDefault(10);										//default value
	discretizationDisplay->setEnabled(false);									//Disabled by default (display cloud point is not selected)
	discretizationDisplay->setHint("Change discretization point cloud step.");	
	
	//Number of divison Geodesic form
	OFX::IntParamDescriptor* nbDivisionsGeodesicForm = desc.defineIntParam(kIntNumberOfDivisonGeodesicForm);
	nbDivisionsGeodesicForm->setLabel(kIntNumberOfDivisonGeodesicFormLabel);	//label
	nbDivisionsGeodesicForm->setRange(2,50);									//value range
	nbDivisionsGeodesicForm->setDisplayRange(2,15);								//display range values
	nbDivisionsGeodesicForm->setDefault(numberOfDivisionInt);					//default value
	nbDivisionsGeodesicForm->setHint("Change precision of treatment (can make process slower)"); //help
	
	
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ColorSpaceKeyerPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                            OFX::EContext context )
{
	return new ColorSpaceKeyerPlugin( handle );
}

}
}
}

