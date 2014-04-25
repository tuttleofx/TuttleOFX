#include "RawReaderPluginFactory.hpp"
#include "RawReaderDefinitions.hpp"
#include "RawReaderPlugin.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/assign/std/vector.hpp>

#include <string>
#include <vector>

namespace tuttle {
namespace plugin {
namespace raw {
namespace reader {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void RawReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
    desc.setLabels(
			"TuttleRawReader",
			"RawReader",
			"Raw file reader"
		);
    desc.setPluginGrouping( "tuttle/image/io" );

	using namespace boost::assign;
    std::vector<std::string> supportedExtensions;
	supportedExtensions += "3fr", "ari", "arw", "bay", "crw", "cr2", "cap", "dng", 
	"dcs", "dcr", "dng", "drf", "eip", "erf", "fff", "iiq", 
	"k25", "kdc", "mef", "mos", "mrw", "nef", "nrw", "obm", 
	"orf", "pef", "ptx", "pxn", "r3d", "rad", "raf", "rw2", 
	"raw", "rwl", "rwz", "srf", "sr2", "srw", "x3f";

    desc.setDescription( "Raw File reader\n"
			"Plugin is used to read raw files.\n\n"
			"supported extensions: \n" +
			boost::algorithm::join( supportedExtensions, ", " )
		);

    // add the supported contexts
    desc.addSupportedContext( OFX::eContextReader );
    desc.addSupportedContext( OFX::eContextGeneral );

    // add supported pixel depths
    desc.addSupportedBitDepth( OFX::eBitDepthFloat );
    desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    desc.addSupportedBitDepth( OFX::eBitDepthUShort );

    // add supported extensions
	desc.addSupportedExtensions( supportedExtensions );
	desc.setPluginEvaluation( 70 );

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
void RawReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
						OFX::EContext               context )
{
    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
    dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
    dstClip->setSupportsTiles( kSupportTiles );

    describeReaderParamsInContext( desc, context );

	OFX::Double2DParamDescriptor* greyboxPoint = desc.defineDouble2DParam( kParamGreyboxPoint);
	greyboxPoint->setLabel( kParamGreyboxPointLabel );
	greyboxPoint->setHint( kParamGreyboxPointHint );
	greyboxPoint->setDisplayRange( 0, 0, 6000, 6000 );
	
	OFX::Double2DParamDescriptor* greyboxSize = desc.defineDouble2DParam( kParamGreyboxSize );
	greyboxSize->setLabel( kParamGreyboxSizeLabel );
	greyboxSize->setHint( kParamGreyboxSizeHint );
	greyboxSize->setDisplayRange( 0, 0, 6000, 6000 );
	
	OFX::DoubleParamDescriptor* redAbber = desc.defineDoubleParam( kParamRedAbber );
	redAbber->setLabel( kParamRedAbberLabel );
	redAbber->setHint( kParamRedAbberHint );
	redAbber->setDefault( 1.0 );
	redAbber->setDisplayRange( 0.999, 1.001 );
	
	OFX::DoubleParamDescriptor* greenAbber = desc.defineDoubleParam( kParamBlueAbber );
	greenAbber->setLabel( kParamBlueAbberLabel );
	greenAbber->setHint( kParamBlueAbberHint );
	greenAbber->setDefault( 1.0 );
	greenAbber->setDisplayRange( 0.999, 1.001 );
	
	OFX::DoubleParamDescriptor* gammaPower = desc.defineDoubleParam( kParamGammaPower );
	gammaPower->setLabel( kParamGammaPowerLabel );
	gammaPower->setHint( kParamGammaPowerHint );
	gammaPower->setDefault( 1.0 );
	gammaPower->setDisplayRange( 0.0, 10.0 );

	OFX::DoubleParamDescriptor* gammaToe = desc.defineDoubleParam( kParamGammaToe );
	gammaToe->setLabel( kParamGammaToeLabel );
	gammaToe->setHint( kParamGammaToeHint );
	gammaToe->setDefault( 1.0 );
	gammaToe->setDisplayRange( 0.0, 10.0 );
	
	OFX::DoubleParamDescriptor* bright = desc.defineDoubleParam( kParamBright );
	bright->setLabel( kParamBrightLabel );
	bright->setHint( kParamBrightHint );
	bright->setDefault( 1.0 );
	bright->setDisplayRange( -8.0, 8.0 );

	OFX::DoubleParamDescriptor* threshold = desc.defineDoubleParam( kParamThreshold );
	threshold->setLabel( kParamThresholdLabel );
	threshold->setHint( kParamThresholdHint );
	threshold->setDefault( 500.0 );
	threshold->setDisplayRange( 100, 1000.0 );
	
	OFX::BooleanParamDescriptor* fourColorRGB = desc.defineBooleanParam( kParamFourColorRgb );
	fourColorRGB->setLabel( kParamFourColorRgbLabel );
	fourColorRGB->setHint( kParamFourColorRgbHint );
	fourColorRGB->setDefault( false );
	
	OFX::ChoiceParamDescriptor* highlight = desc.defineChoiceParam( kParamHighlight );
	highlight->setLabel( kParamHighlightLabel );
	highlight->setHint( kParamHighlightHint );
	highlight->appendOption( kParamHighlightClip );
	highlight->appendOption( kParamHighlightUnclip );
	highlight->appendOption( kParamHighlightBlend );
	highlight->appendOption( kParamHighlightRebuild1 );
	highlight->appendOption( kParamHighlightRebuild2 );
	highlight->appendOption( kParamHighlightRebuild3 );
	highlight->appendOption( kParamHighlightRebuild4 );
	highlight->appendOption( kParamHighlightRebuild5 );
	highlight->appendOption( kParamHighlightRebuild6 );
	highlight->appendOption( kParamHighlightRebuild7 );
	highlight->setDefault( eHighlightBlend );
	
	
	OFX::ChoiceParamDescriptor* interpolation = desc.defineChoiceParam( kParamInterpolation );
	interpolation->setLabel( kParamInterpolationLabel );
	interpolation->setHint( kParamInterpolationHint );
	interpolation->appendOption( kParamInterpolationLinear );
	interpolation->appendOption( kParamInterpolationVng );
	interpolation->appendOption( kParamInterpolationPpg );
	interpolation->appendOption( kParamInterpolationAhd );
	interpolation->appendOption( kParamInterpolationDcb );
	interpolation->appendOption( kParamInterpolationModifiedAhd );
	interpolation->appendOption( kParamInterpolationAfd );
	interpolation->appendOption( kParamInterpolationVcd );
	interpolation->appendOption( kParamInterpolationMixed );
	interpolation->appendOption( kParamInterpolationLmmse );
	interpolation->appendOption( kParamInterpolationAmaze );
	interpolation->setDefault( 3 );
	
	OFX::DoubleParamDescriptor* exposure = desc.defineDoubleParam( kParamExposure );
	exposure->setLabel( kParamExposureLabel );
	exposure->setHint( kParamExposureHint );
	exposure->setDefault( 1.0 );
	exposure->setDisplayRange( 0.0, 10.0 );
	
	OFX::DoubleParamDescriptor* preserve = desc.defineDoubleParam( kParamExposurePreserve );
	preserve->setLabel( kParamThresholdLabel );
	preserve->setHint( kParamThresholdHint );
	preserve->setDefault( 0.0 );
	preserve->setDisplayRange( 0, 1.0 );
	
	OFX::ChoiceParamDescriptor* whitebalance = desc.defineChoiceParam( kParamWhiteBalance );
	whitebalance->setLabel( kParamWhiteBalanceLabel );
	whitebalance->setHint( kParamWhiteBalanceHint );
	whitebalance->appendOption( kParamWhiteBalanceAutoWb );
	whitebalance->appendOption( kParamWhiteBalanceCameraWb );
#ifndef TUTTLE_PRODUCTION
	whitebalance->appendOption( kParamWhiteBalanceManualWb );
	whitebalance->appendOption( kParamWhiteBalance2500 );
	whitebalance->appendOption( kParamWhiteBalance2550 );
	whitebalance->appendOption( kParamWhiteBalance2650 );
	whitebalance->appendOption( kParamWhiteBalance2700 );
	whitebalance->appendOption( kParamWhiteBalance2800 );
	whitebalance->appendOption( kParamWhiteBalance2850 );
	whitebalance->appendOption( kParamWhiteBalance2950 );
	whitebalance->appendOption( kParamWhiteBalance3000 );
	whitebalance->appendOption( kParamWhiteBalance3100 );
	whitebalance->appendOption( kParamWhiteBalance3200 );
	whitebalance->appendOption( kParamWhiteBalance3300 );
	whitebalance->appendOption( kParamWhiteBalance3400 );
	whitebalance->appendOption( kParamWhiteBalance3600 );
	whitebalance->appendOption( kParamWhiteBalance3700 );
	whitebalance->appendOption( kParamWhiteBalance3800 );
	whitebalance->appendOption( kParamWhiteBalance4000 );
	whitebalance->appendOption( kParamWhiteBalance4200 );
	whitebalance->appendOption( kParamWhiteBalance4300 );
	whitebalance->appendOption( kParamWhiteBalance4500 );
	whitebalance->appendOption( kParamWhiteBalance4800 );
	whitebalance->appendOption( kParamWhiteBalance5000 );
	whitebalance->appendOption( kParamWhiteBalance5300 );
	whitebalance->appendOption( kParamWhiteBalance5600 );
	whitebalance->appendOption( kParamWhiteBalance5900 );
	whitebalance->appendOption( kParamWhiteBalance6300 );
	whitebalance->appendOption( kParamWhiteBalance6700 );
	whitebalance->appendOption( kParamWhiteBalance7100 );
	whitebalance->appendOption( kParamWhiteBalance7700 );
	whitebalance->appendOption( kParamWhiteBalance8300 );
	whitebalance->appendOption( kParamWhiteBalance9100 );
	whitebalance->appendOption( kParamWhiteBalance10000 );
#endif
	whitebalance->setDefault( eCameraWb );
	
	OFX::ChoiceParamDescriptor* filtering = desc.defineChoiceParam( kParamFiltering );
	filtering->setLabel( "Filtering" );
	filtering->appendOption( kParamFilteringAuto );
	filtering->appendOption( kParamFilteringNone );
	
	OFX::GroupParamDescriptor* metadata = desc.defineGroupParam( kParamMetadata );
	metadata->setLabel( kParamMetadata );
	
	OFX::StringParamDescriptor* manufacturer = desc.defineStringParam( kParamManufacturer );
	manufacturer->setLabel( kParamManufacturerLabel );
	manufacturer->setHint( kParamManufacturerHint );
	manufacturer->setEnabled( false );
	manufacturer->setParent( metadata );
	
	OFX::StringParamDescriptor* model = desc.defineStringParam( kParamModel );
	model->setLabel( kParamModelLabel );
	model->setHint( kParamModelHint );
	model->setEnabled( false );
	model->setParent( metadata );

	OFX::IntParamDescriptor* iso = desc.defineIntParam( kParamIso );
	iso->setLabel( kParamIsoLabel );
	iso->setHint( kParamIsoHint );
	iso->setEnabled( false );
	iso->setParent( metadata );
	
	OFX::IntParamDescriptor* shutter = desc.defineIntParam( kParamShutter );
	shutter->setLabel( kParamShutterLabel );
	shutter->setHint( kParamShutterHint );
	shutter->setEnabled( false );
	shutter->setParent( metadata );
	
	OFX::DoubleParamDescriptor* aperture = desc.defineDoubleParam( kParamAperture );
	aperture->setLabel( kParamApertureLabel );
	aperture->setHint( kParamApertureHint );
	aperture->setEnabled( false );
	aperture->setParent( metadata );
	
	OFX::StringParamDescriptor* date = desc.defineStringParam( kParamDateOfShooting );
	date->setLabel( kParamDateOfShootingLabel );
	date->setHint( kParamDateOfShootingHint );
	date->setEnabled( false );
	date->setParent( metadata );
	
	OFX::StringParamDescriptor* gps = desc.defineStringParam( kParamGPS );
	gps->setLabel( kParamGPSLabel );
	gps->setHint( kParamGPSHint );
	gps->setEnabled( false );
	gps->setParent( metadata );
	
	OFX::StringParamDescriptor* description = desc.defineStringParam( kParamDesc );
	description->setLabel( kParamDescLabel );
	description->setHint( kParamDescHint );
	description->setEnabled( false );
	description->setParent( metadata );
	
	OFX::StringParamDescriptor* artist = desc.defineStringParam( kParamArtist );
	artist->setLabel( kParamArtistLabel );
	artist->setHint( kParamArtistHint );
	artist->setEnabled( false );
	artist->setParent( metadata );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context  Application context
 * @return  plugin instance
 */
OFX::ImageEffect* RawReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
							  OFX::EContext        context )
{
    return new RawReaderPlugin( handle );
}

}
}
}
}
