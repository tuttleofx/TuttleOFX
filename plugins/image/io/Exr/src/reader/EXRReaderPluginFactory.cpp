#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPluginFactory.hpp"
#include "EXRReaderPlugin.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void EXRReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels( "TuttleExrReader", "ExrReader",
			"Exr file reader" );
	desc.setPluginGrouping( "tuttle/image/io" );

	std::vector<std::string> extension;
	extension.push_back( "exr" );
	
	std::string listOfExt;
	for( unsigned int i=0; i< extension.size(); i++ )
	{
		listOfExt += extension.at(i);
		listOfExt += ", ";
	}
	listOfExt.erase( listOfExt.size()-2, 2 );
	
	desc.setDescription( "EXR File reader\n"
				 "Plugin is used to read exr files.\n\n"
				 "supported extensions: \n" +
				 listOfExt
	);

	// add the supported contexts
	desc.addSupportedContext( OFX::eContextReader );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

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
void EXRReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
						OFX::EContext               context )
{
	OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
	dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
	dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
	dstClip->setSupportsTiles( kSupportTiles );

	describeReaderParamsInContext( desc, context );

	OFX::ChoiceParamDescriptor* outRedIs = desc.defineChoiceParam( kParamOutputRedIs );
	outRedIs->appendOption( "0" );
	outRedIs->appendOption( "1" );
	outRedIs->appendOption( "2" );
	outRedIs->appendOption( "3" );
	outRedIs->setLabel( "Red is" );
	outRedIs->setDefault( 0 );

	OFX::ChoiceParamDescriptor* outGreenIs = desc.defineChoiceParam( kParamOutputGreenIs );
	outGreenIs->appendOption( "0" );
	outGreenIs->appendOption( "1" );
	outGreenIs->appendOption( "2" );
	outGreenIs->appendOption( "3" );
	outGreenIs->setLabel( "Green is" );
	outGreenIs->setDefault( 0 );

	OFX::ChoiceParamDescriptor* outBlueIs = desc.defineChoiceParam( kParamOutputBlueIs );
	outBlueIs->appendOption( "0" );
	outBlueIs->appendOption( "1" );
	outBlueIs->appendOption( "2" );
	outBlueIs->appendOption( "3" );
	outBlueIs->setLabel( "Blue is" );
	outBlueIs->setDefault( 0 );

	OFX::ChoiceParamDescriptor* outAlphaIs = desc.defineChoiceParam( kParamOutputAlphaIs );
	outAlphaIs->appendOption( "0" );
	outAlphaIs->appendOption( "1" );
	outAlphaIs->appendOption( "2" );
	outAlphaIs->appendOption( "3" );
	outAlphaIs->setLabel( "Alpha is" );
	outAlphaIs->setDefault( 0 );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* EXRReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
							  OFX::EContext        context )
{
	return new EXRReaderPlugin( handle );
}

}
}
}
}
