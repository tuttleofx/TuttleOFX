#include "RawReaderPluginFactory.hpp"
#include "RawReaderDefinitions.hpp"
#include "RawReaderPlugin.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>

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
    desc.setLabels( "TuttleRawReader", "RawReader",
		    "Raw file reader" );
    desc.setPluginGrouping( "tuttle/image/io" );

    std::vector<std::string> extension;
    extension.push_back( "3fr" );
    extension.push_back( "ari" );
    extension.push_back( "arw" );
    extension.push_back( "bay" );
    extension.push_back( "crw" );
    extension.push_back( "cr2" );
    extension.push_back( "cap" );
    extension.push_back( "dng" );
    extension.push_back( "dcs" );
    extension.push_back( "dcr" );
    extension.push_back( "dng" );
    extension.push_back( "drf" );
    extension.push_back( "eip" );
    extension.push_back( "erf" );
    extension.push_back( "fff" );
    extension.push_back( "iiq" );
    extension.push_back( "k25" );
    extension.push_back( "kdc" );
    extension.push_back( "mef" );
    extension.push_back( "mos" );
    extension.push_back( "mrw" );
    extension.push_back( "nef" );
    extension.push_back( "nrw" );
    extension.push_back( "obm" );
    extension.push_back( "orf" );
    extension.push_back( "pef" );
    extension.push_back( "ptx" );
    extension.push_back( "pxn" );
    extension.push_back( "r3d" );
    extension.push_back( "rad" );
    extension.push_back( "raf" );
    extension.push_back( "rw2" );
    extension.push_back( "raw" );
    extension.push_back( "rwl" );
    extension.push_back( "rwz" );
    extension.push_back( "srf" );
    extension.push_back( "sr2" );
    extension.push_back( "srw" );
    extension.push_back( "x3f" );

    std::string listOfExt;
    for( unsigned int i=0; i< extension.size(); i++ )
    {
	listOfExt += extension.at(i);
	listOfExt += ", ";
    }
    listOfExt.erase( listOfExt.size()-2, 2 );
    desc.setDescription( "Raw File reader\n"
			 "Plugin is used to read raw files.\n\n"
			 "supported extensions: \n" +
			 listOfExt
			 );

    // add the supported contexts
    desc.addSupportedContext( OFX::eContextReader );
    desc.addSupportedContext( OFX::eContextGeneral );

    // add supported pixel depths
    desc.addSupportedBitDepth( OFX::eBitDepthFloat );
    desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    desc.addSupportedBitDepth( OFX::eBitDepthUShort );

    // add supported extensions
    for( unsigned int i=0; i< extension.size(); i++ )
    {
	desc.addSupportedExtension( extension.at(i) );
    }

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
	redAbber->setDisplayRange( 0.99, 1.01 );
	
	OFX::DoubleParamDescriptor* greenAbber = desc.defineDoubleParam( kParamGreenAbber );
	greenAbber->setLabel( kParamGreenAbberLabel );
	greenAbber->setHint( kParamGreenAbberHint );
	greenAbber->setDefault( 1.0 );
	greenAbber->setDisplayRange( 0.99, 1.01 );
	
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
	bright->setDisplayRange( 0.0, 1.0 );
	
	OFX::ChoiceParamDescriptor* interpolation = desc.defineChoiceParam( kParamInterpolation );
	interpolation->setLabel( kParamInterpolationLabel );
	interpolation->setLabel( kParamInterpolationHint );
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
	
    OFX::ChoiceParamDescriptor* filtering = desc.defineChoiceParam( kParamFiltering );
    filtering->setLabel( "Filtering" );
    filtering->appendOption( kParamFilteringAuto );
    filtering->appendOption( kParamFilteringNone );
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
