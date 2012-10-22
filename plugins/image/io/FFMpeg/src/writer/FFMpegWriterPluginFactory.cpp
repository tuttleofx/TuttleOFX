#include "FFMpegWriterPluginFactory.hpp"
#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterDefinitions.hpp"

#include <ffmpeg/VideoFFmpegWriter.hpp>

#include <tuttle/plugin/context/WriterPluginFactory.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/foreach.hpp>

#include <string>
#include <vector>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void FFMpegWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
	desc.setLabels(
		"TuttleFfmpegWriter",
		"FfmpegWriter",
		"Ffmpeg video writer" );
	desc.setPluginGrouping( "tuttle/image/io" );

	std::vector<std::string> supportedExtensions;
	{
		AVOutputFormat* oFormat = av_oformat_next( NULL );
		while( oFormat != NULL )
		{
			if( oFormat->extensions != NULL )
			{
				using namespace boost::algorithm;
				const std::string extStr( oFormat->extensions );
				std::vector<std::string> exts;
				split( exts, extStr, is_any_of(",") );
				
				// remove empty extensions...
				for( std::vector<std::string>::iterator it = exts.begin(); it != exts.end(); )
				{
					if( it->size() == 0 )
						it = exts.erase( it );
					else
						++it;
				}
				supportedExtensions.insert( supportedExtensions.end(), exts.begin(), exts.end() );
			}
			oFormat = av_oformat_next( oFormat );
		}
	}
	
	desc.setDescription( "Video writer based on FFMpeg library\n\n"
			"Supported extensions: \n" +
			boost::algorithm::join( supportedExtensions, ", " )
		);
	
	// add the supported contexts
	desc.addSupportedContext( OFX::eContextWriter );
	desc.addSupportedContext( OFX::eContextGeneral );

	// add supported pixel depths
	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
	desc.addSupportedBitDepth( OFX::eBitDepthFloat );

    // add supported extensions
	desc.addSupportedExtensions( supportedExtensions );
	
	// plugin flags
	desc.setRenderThreadSafety( OFX::eRenderInstanceSafe );
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
void FFMpegWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
						   OFX::EContext               context )
{
	VideoFFmpegWriter writer;

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
	
	// Controls
	describeWriterParamsInContext( desc, context );
	OFX::ChoiceParamDescriptor* bitDepth = static_cast<OFX::ChoiceParamDescriptor*>( desc.getParamDescriptor( kTuttlePluginBitDepth ) );
	bitDepth->appendOption( kTuttlePluginBitDepth8 );
	/// @todo: 16 bits
	bitDepth->setDefault( eTuttlePluginBitDepth8 );
	bitDepth->setEnabled( false );

	OFX::BooleanParamDescriptor* premult = static_cast<OFX::BooleanParamDescriptor*>( desc.getParamDescriptor( kParamPremultiplied ) );
	premult->setDefault( true );

	// Groups
	OFX::GroupParamDescriptor* formatGroup = desc.defineGroupParam( kParamFormatGroup );
	OFX::GroupParamDescriptor* videoGroup  = desc.defineGroupParam( kParamVideoGroup );
	OFX::GroupParamDescriptor* audioGroup  = desc.defineGroupParam( kParamAudioGroup );
	OFX::GroupParamDescriptor* metaGroup   = desc.defineGroupParam( kParamMetaGroup );
	
	formatGroup->setAsTab( );
	videoGroup->setAsTab( );
	audioGroup->setAsTab( );
	metaGroup->setAsTab( );

	int default_format = 0;
	OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamFormat );
	for( std::vector<std::string>::const_iterator itShort = writer.getFormatsShort().begin(),
		itLong = writer.getFormatsLong().begin(),
		itEnd = writer.getFormatsShort().end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		format->appendOption( *itShort, *itLong );
		if( (*itShort) == "mp4" )
			default_format = format->getNOptions() - 1;
	}
	format->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	format->setDefault( default_format );
	format->setParent( formatGroup );

	int default_codec = 0;
	OFX::ChoiceParamDescriptor* videoCodec = desc.defineChoiceParam( kParamVideoCodec );
	for( std::vector<std::string>::const_iterator itShort = writer.getVideoCodecsShort().begin(),
		itLong  = writer.getVideoCodecsLong().begin(),
		itEnd = writer.getVideoCodecsShort().end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		videoCodec->appendOption( *itShort, *itLong );
		if( (*itShort) == "mpeg4" )
			default_codec = videoCodec->getNOptions() - 1;
	}
	videoCodec->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	videoCodec->setDefault( default_codec );

	
	const std::vector<std::string> codecListWithPreset = writer.getPresets().getCodecListWithConfig();
	BOOST_FOREACH( const std::string& codecName, codecListWithPreset )
	{
		const std::vector<std::string> codecList = writer.getPresets().getConfigList( codecName );
		if( codecList.empty() )
			continue;
		
		OFX::ChoiceParamDescriptor* choiceConfig = desc.defineChoiceParam( codecName + kParamPresetPostfix );
		choiceConfig->setLabel( codecName + " Preset" );
		choiceConfig->setDefault( 0 );
		
		BOOST_FOREACH( const std::string& codec, codecList )
		{
			choiceConfig->appendOption( codec );
		}
	}
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* FFMpegWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
							     OFX::EContext        context )
{
	return new FFMpegWriterPlugin( handle );
}

}
}
}
}
