#include "ColorSpacePluginFactory.hpp"
#include "ColorSpacePlugin.hpp"
#include "ColorSpaceDefinitions.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>


namespace tuttle {
namespace plugin {
namespace colorspace {

namespace ttlc = tuttle::plugin::color;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void ColorSpacePluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleColorSpace", "ColorSpace",
					"Color convertions" );
	desc.setPluginGrouping( "tuttle/image/process/color" );

	desc.setDescription( "ColorSpace\n"
			 "Plugin is used to apply colorspace transformations on images." );

	// add the supported contexts
	desc.addSupportedContext		( OFX::eContextFilter );
	desc.addSupportedContext		( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth		( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth		( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth		( OFX::eBitDepthFloat );

	// plugin flags
	desc.setSupportsTiles			( kSupportTiles );
	desc.setRenderThreadSafety		( OFX::eRenderFullySafe );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ColorSpacePluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
						 OFX::EContext               context )
{
	OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );
	srcClip->addSupportedComponent		( OFX::ePixelComponentRGBA );
	srcClip->addSupportedComponent		( OFX::ePixelComponentRGB );
	srcClip->addSupportedComponent		( OFX::ePixelComponentAlpha );
	srcClip->setSupportsTiles		( kSupportTiles );

	// Create the mandated output clip
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent		( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent		( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent		( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles		( kSupportTiles );

	/* ----------------------- INPUT PARAMETERS -------------------------- */

	OFX::GroupParamDescriptor* inGroup = desc.defineGroupParam( kColorSpaceIn );
	inGroup->setLabel			( "Input configuration" );

	OFX::ChoiceParamDescriptor* inReferenceSpace = desc.defineChoiceParam( kColorSpaceReferenceSpaceIn );
	inReferenceSpace->setLabel		( "Reference Space" );
	inReferenceSpace->setParent		( inGroup );

	OFX::GroupParamDescriptor* inCustom = desc.defineGroupParam( kColorSpaceCustomizedIn );
	inCustom->setLabel			( "Custom" );
	inCustom->setOpen			( false );
	inCustom->setParent			( inGroup );

	OFX::ChoiceParamDescriptor* inGradationLaw = desc.defineChoiceParam( kColorSpaceGradationLawIn );
	inGradationLaw->setLabel		( "Gradation Law" );
	inGradationLaw->setParent		( inCustom );

	OFX::DoubleParamDescriptor* inGammaValue = desc.defineDoubleParam( kColorSpaceInGammaValue );
	inGammaValue->setLabel			( "Gamma" );
	inGammaValue->setDefault		( 1.0 );
	inGammaValue->setRange			( 0.0, std::numeric_limits<double>::max() );
	inGammaValue->setDisplayRange		( 0.0, 5.0 );
	inGammaValue->setHint			( "Adjust the Gamma." );
	inGammaValue->setParent			( inCustom );

	OFX::DoubleParamDescriptor* inBlackPoint = desc.defineDoubleParam( kColorSpaceInBlackPoint );
	inBlackPoint->setLabel			( "Black Point" );
	inBlackPoint->setDefault		( 0.0 );
	inBlackPoint->setRange			( 0.0, 1.0 );
	inBlackPoint->setDisplayRange		( 0.0, 1.0 );
	inBlackPoint->setHint			( "Adjust the Black Point." );
	inBlackPoint->setParent			( inCustom );

	OFX::DoubleParamDescriptor* inWhitePoint = desc.defineDoubleParam( kColorSpaceInWhitePoint );
	inWhitePoint->setLabel			( "White Point" );
	inWhitePoint->setDefault		( 1.0 );
	inWhitePoint->setRange			( 0.0, 1.0 );
	inWhitePoint->setDisplayRange		( 0.0, 1.0 );
	inWhitePoint->setHint			( "Adjust the White Point." );
	inWhitePoint->setParent			( inCustom );

	OFX::DoubleParamDescriptor* inGammaSensito = desc.defineDoubleParam( kColorSpaceInGammaSensito );
	inGammaSensito->setLabel		( "Gamma Sensito" );
	inGammaSensito->setDefault		( 1.0 );
	inGammaSensito->setRange		( 0.0, std::numeric_limits<double>::max() );
	inGammaSensito->setDisplayRange		( 0.0, 5.0 );
	inGammaSensito->setHint			( "Adjust the Gamma Sensito." );
	inGammaSensito->setParent		( inCustom );

	OFX::ChoiceParamDescriptor* inLayout = desc.defineChoiceParam( kColorSpaceLayoutIn );
	inLayout->setLabel			( "Layout" );
	inLayout->setParent			( inCustom );

	OFX::ChoiceParamDescriptor* inTempColor = desc.defineChoiceParam( kColorSpaceTempColorIn );
	inTempColor->setLabel			( "Color Temperature" );
	inTempColor->setHint			( "Select the color temperature." );
	inTempColor->setParent			( inCustom );

	OFX::GroupParamDescriptor* inPrimaries = desc.defineGroupParam( kColorSpacePrimariesIn );
	inPrimaries->setLabel			( "Primaries color" );
	inPrimaries->setOpen			( false );
	inPrimaries->setParent			( inCustom );

	OFX::DoubleParamDescriptor* inXr = desc.defineDoubleParam( kColorSpaceXrIn );
	inXr->setLabel				( "X red" );
	inXr->setDefault			( 1.0 );
	inXr->setRange				( 0.0, 1.0 );
	inXr->setDisplayRange			( 0.0, 1.0 );
	inXr->setHint				( "Adjust the X red primary color." );
	inXr->setParent				( inPrimaries );

	OFX::DoubleParamDescriptor* inYr = desc.defineDoubleParam( kColorSpaceYrIn );
	inYr->setLabel				( "Y red" );
	inYr->setDefault			( 1.0 );
	inYr->setRange				( 0.0, 1.0 );
	inYr->setDisplayRange			( 0.0, 1.0 );
	inYr->setHint				( "Adjust the Y red primary color." );
	inYr->setParent				( inPrimaries );

	OFX::DoubleParamDescriptor* inXg = desc.defineDoubleParam( kColorSpaceXgIn );
	inXg->setLabel				( "X green" );
	inXg->setDefault			( 1.0 );
	inXg->setRange				( 0.0, 1.0 );
	inXg->setDisplayRange			( 0.0, 1.0 );
	inXg->setHint				( "Adjust the X green primary color." );
	inXg->setParent				( inPrimaries );

	OFX::DoubleParamDescriptor* inYg = desc.defineDoubleParam( kColorSpaceYgIn );
	inYg->setLabel				( "Y green" );
	inYg->setDefault			( 1.0 );
	inYg->setRange				( 0.0, 1.0 );
	inYg->setDisplayRange			( 0.0, 1.0 );
	inYg->setHint				( "Adjust the Y green primary color." );
	inYg->setParent				( inPrimaries );

	OFX::DoubleParamDescriptor* inXb = desc.defineDoubleParam( kColorSpaceXbIn );
	inXb->setLabel				( "X blue" );
	inXb->setDefault			( 1.0 );
	inXb->setRange				( 0.0, 1.0 );
	inXb->setDisplayRange			( 0.0, 1.0 );
	inXb->setHint				( "Adjust the X blue primary color." );
	inXb->setParent				( inPrimaries );

	OFX::DoubleParamDescriptor* inYb = desc.defineDoubleParam( kColorSpaceYbIn );
	inYb->setLabel				( "Y blue" );
	inYb->setDefault			( 1.0 );
	inYb->setRange				( 0.0, 1.0 );
	inYb->setDisplayRange			( 0.0, 1.0 );
	inYb->setHint				( "Adjust the Y blue primary color." );
	inYb->setParent				( inPrimaries );

	/* ----------------------- OUTPUT PARAMETERS -------------------------- */

	OFX::GroupParamDescriptor* outGroup = desc.defineGroupParam( kColorSpaceOut );
	outGroup->setLabel			( "Output configuration" );

	OFX::ChoiceParamDescriptor* outReferenceSpace = desc.defineChoiceParam( kColorSpaceReferenceSpaceOut );
	outReferenceSpace->setLabel		( "Reference Space" );
	outReferenceSpace->setParent		( outGroup );

	OFX::GroupParamDescriptor* outCustom = desc.defineGroupParam( kColorSpaceCustomizedOut );
	outCustom->setLabel			( "Custom" );
	outCustom->setOpen			( false );
	outCustom->setParent			( outGroup );

	OFX::ChoiceParamDescriptor* outGradationLaw = desc.defineChoiceParam( kColorSpaceGradationLawOut );
	outGradationLaw->setLabel		( "Gradation Law" );
	outGradationLaw->setParent		( outCustom );

	OFX::DoubleParamDescriptor* outGammaValue = desc.defineDoubleParam( kColorSpaceOutGammaValue );
	outGammaValue->setLabel			( "Gamma" );
	outGammaValue->setDefault		( 1.0 );
	outGammaValue->setRange			( 0.0, std::numeric_limits<double>::max() );
	outGammaValue->setDisplayRange		( 0.0, 5.0 );
	outGammaValue->setHint			( "Adjust the Gamma." );
	outGammaValue->setParent		( outCustom );

	OFX::DoubleParamDescriptor* outBlackPoint = desc.defineDoubleParam( kColorSpaceOutBlackPoint );
	outBlackPoint->setLabel			( "Black Point" );
	outBlackPoint->setDefault		( 0.0 );
	outBlackPoint->setRange			( 0.0, 1.0 );
	outBlackPoint->setDisplayRange		( 0.0, 1.0 );
	outBlackPoint->setHint			( "Adjust the Black Point." );
	outBlackPoint->setParent		( outCustom );

	OFX::DoubleParamDescriptor* outWhitePoint = desc.defineDoubleParam( kColorSpaceOutWhitePoint );
	outWhitePoint->setLabel			( "White Point" );
	outWhitePoint->setDefault		( 1.0 );
	outWhitePoint->setRange			( 0.0, 1.0 );
	outWhitePoint->setDisplayRange		( 0.0, 1.0 );
	outWhitePoint->setHint			( "Adjust the White Point." );
	outWhitePoint->setParent		( outCustom );

	OFX::DoubleParamDescriptor* outGammaSensito = desc.defineDoubleParam( kColorSpaceOutGammaSensito );
	outGammaSensito->setLabel		( "Gamma Sensito" );
	outGammaSensito->setDefault		( 1.0 );
	outGammaSensito->setRange		( 0.0, std::numeric_limits<double>::max() );
	outGammaSensito->setDisplayRange	( 0.0, 5.0 );
	outGammaSensito->setHint		( "Adjust the Gamma Sensito." );
	outGammaSensito->setParent		( outCustom );

	OFX::ChoiceParamDescriptor* outLayout = desc.defineChoiceParam( kColorSpaceLayoutOut );
	outLayout->setLabel			( "Layout" );
	outLayout->setParent			( outCustom );

	OFX::ChoiceParamDescriptor* outTempColor = desc.defineChoiceParam( kColorSpaceTempColorOut );
	outTempColor->setLabel			( "Color Temperature" );
	outTempColor->setHint			( "Select the color temperature." );
	outTempColor->setParent			( outCustom );

	OFX::GroupParamDescriptor* outPrimaries = desc.defineGroupParam( kColorSpacePrimariesOut );
	outPrimaries->setLabel			( "Primaries color" );
	outPrimaries->setOpen			( false );
	outPrimaries->setParent			( outCustom );

	OFX::DoubleParamDescriptor* outXr = desc.defineDoubleParam( kColorSpaceXrOut );
	outXr->setLabel				( "X red" );
	outXr->setDefault			( 1.0 );
	outXr->setRange				( 0.0, 1.0 );
	outXr->setDisplayRange			( 0.0, 1.0 );
	outXr->setHint				( "Adjust the X red primary color." );
	outXr->setParent			( outPrimaries );

	OFX::DoubleParamDescriptor* outYr = desc.defineDoubleParam( kColorSpaceYrOut );
	outYr->setLabel				( "Y red" );
	outYr->setDefault			( 1.0 );
	outYr->setRange				( 0.0, 1.0 );
	outYr->setDisplayRange			( 0.0, 1.0 );
	outYr->setHint				( "Adjust the Y red primary color." );
	outYr->setParent			( outPrimaries );

	OFX::DoubleParamDescriptor* outXg = desc.defineDoubleParam( kColorSpaceXgOut );
	outXg->setLabel				( "X green" );
	outXg->setDefault			( 1.0 );
	outXg->setRange				( 0.0, 1.0 );
	outXg->setDisplayRange			( 0.0, 1.0 );
	outXg->setHint				( "Adjust the X green primary color." );
	outXg->setParent			( outPrimaries );

	OFX::DoubleParamDescriptor* outYg = desc.defineDoubleParam( kColorSpaceYgOut );
	outYg->setLabel				( "Y green" );
	outYg->setDefault			( 1.0 );
	outYg->setRange				( 0.0, 1.0 );
	outYg->setDisplayRange			( 0.0, 1.0 );
	outYg->setHint				( "Adjust the Y green primary color." );
	outYg->setParent			( outPrimaries );

	OFX::DoubleParamDescriptor* outXb = desc.defineDoubleParam( kColorSpaceXbOut );
	outXb->setLabel				( "X blue" );
	outXb->setDefault			( 1.0 );
	outXb->setRange				( 0.0, 1.0 );
	outXb->setDisplayRange			( 0.0, 1.0 );
	outXb->setHint				( "Adjust the X blue primary color." );
	outXb->setParent			( outPrimaries );

	OFX::DoubleParamDescriptor* outYb = desc.defineDoubleParam( kColorSpaceYbOut );
	outYb->setLabel				( "Y blue" );
	outYb->setDefault			( 1.0 );
	outYb->setRange				( 0.0, 1.0 );
	outYb->setDisplayRange			( 0.0, 1.0 );
	outYb->setHint				( "Adjust the Y blue primary color." );
	outYb->setParent			( outPrimaries );

	/* -------------- ENUMS FILLING ----------------*/

	ttlc::ColorSpaceMaps csMaps;
	ttlc::ColorSpaceMap mapReferenceSpace	= csMaps.getMapReferenceSpaces();
	ttlc::ColorSpaceMap mapGradationLaw	= csMaps.getMapGradationLaw();
	ttlc::ColorSpaceMap mapLayout		= csMaps.getMapLayout();
	ttlc::ColorSpaceMap mapColourTemp	= csMaps.getMapColourTemp();
	ttlc::ColorSpaceMap::iterator it;
	ttlc::ColorSpacePair highest;

	/* Reference Space */
	highest	= *mapReferenceSpace.rbegin();		// last element
	it	=  mapReferenceSpace.begin();
	do {
		inReferenceSpace->appendOption	( (*it).second );
		outReferenceSpace->appendOption	( (*it).second );

	} while ( mapReferenceSpace.value_comp()(*it++, highest) );

	it = mapReferenceSpace.begin();
	inReferenceSpace->setDefault	( (*it).first );
	outReferenceSpace->setDefault	( (*it).first );

	/* Gradation */
	highest	= *mapGradationLaw.rbegin();
	it	=  mapGradationLaw.begin();
	do {
		inGradationLaw->appendOption	( (*it).second );
		outGradationLaw->appendOption	( (*it).second );

	} while ( mapGradationLaw.value_comp()(*it++, highest) );

	it = mapGradationLaw.begin();
	inGradationLaw->setDefault	( (*it).first );
	outGradationLaw->setDefault	( (*it).first );

	/* Layout */
	highest	= *mapLayout.rbegin();
	it	=  mapLayout.begin();
	do {
		inLayout->appendOption	( (*it).second );
		outLayout->appendOption	( (*it).second );

	} while ( mapLayout.value_comp()(*it++, highest) );

	it = mapLayout.begin();
	inLayout->setDefault	( (*it).first );
	outLayout->setDefault	( (*it).first );

	/* Colour temperature */
	highest	= *mapColourTemp.rbegin();
	it	=  mapColourTemp.begin();
	do {
		inTempColor->appendOption	( (*it).second );
		outTempColor->appendOption	( (*it).second );

	} while ( mapColourTemp.value_comp()(*it++, highest) );

	it = mapColourTemp.begin();
	inTempColor->setDefault	( (*it).first );
	outTempColor->setDefault	( (*it).first );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ColorSpacePluginFactory::createInstance( OfxImageEffectHandle handle,
							   OFX::EContext        context )
{
	return new ColorSpacePlugin( handle );
}

}
}
}
