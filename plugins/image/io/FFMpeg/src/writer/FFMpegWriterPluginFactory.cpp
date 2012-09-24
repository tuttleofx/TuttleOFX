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

namespace {

void declareAVPrivOptions( OFX::ImageEffectDescriptor& desc, const std::vector<AVPrivOption>& avPrivOpts, OFX::GroupParamDescriptor* groupParams )
{
	BOOST_FOREACH( const AVPrivOption& opt, avPrivOpts )
	{
		if( opt.o.name == NULL )
			continue;
		
		OFX::ValueParamDescriptor* curOpt;
		switch( opt.o.type )
		{
			case AV_OPT_TYPE_FLAGS:
			{
				OFX::BooleanParamDescriptor* curOptBoolean = desc.defineBooleanParam( opt.class_name + "_" + opt.o.name );
				curOpt = curOptBoolean;
				curOptBoolean->setLabel( opt.o.name );
				curOptBoolean->setDefault( opt.o.default_val.dbl );
				if( opt.o.help )
					curOptBoolean->setHint( opt.o.help );
				break;
			}
			case AV_OPT_TYPE_INT:
			{
				OFX::IntParamDescriptor* curOptInt = desc.defineIntParam( opt.class_name + "_" + opt.o.name );
				curOpt = curOptInt;
				curOptInt->setLabel( opt.o.name );
				curOptInt->setRange( opt.o.min, opt.o.max );
				curOptInt->setDisplayRange( opt.o.min, opt.o.max );
				curOptInt->setDefault( opt.o.default_val.dbl );
				if( opt.o.help )
					curOptInt->setHint( opt.o.help );
				break;
			}
			case AV_OPT_TYPE_FLOAT:
			case AV_OPT_TYPE_DOUBLE:
			{
				OFX::DoubleParamDescriptor* curOptDouble = desc.defineDoubleParam( opt.class_name + "_" + opt.o.name );
				curOpt = curOptDouble;
				curOptDouble->setLabel( opt.o.name );
				curOptDouble->setRange( opt.o.min, opt.o.max );
				curOptDouble->setDisplayRange( opt.o.min, opt.o.max );
				curOptDouble->setDefault( opt.o.default_val.dbl );
				if( opt.o.help )
					curOptDouble->setHint( opt.o.help );
				break;
			}
			case AV_OPT_TYPE_STRING:
			{
				OFX::StringParamDescriptor* curOptString = desc.defineStringParam( opt.class_name + "_" + opt.o.name );
				curOpt = curOptString;
				curOptString->setLabel( opt.o.name );
				if( opt.o.help )
					curOptString->setHint( opt.o.help );
				break;
			}
			case AV_OPT_TYPE_IMAGE_SIZE:
			case AV_OPT_TYPE_RATIONAL:
			{
				OFX::Int2DParamDescriptor* curOptInt2D = desc.defineInt2DParam( opt.class_name + "_" + opt.o.name );
				curOpt = curOptInt2D;
				curOptInt2D->setLabel( opt.o.name );
				curOptInt2D->setRange( opt.o.min, opt.o.max, opt.o.min, opt.o.max );
				curOptInt2D->setDisplayRange( opt.o.min, opt.o.max, opt.o.min, opt.o.max );
				curOptInt2D->setDefault( opt.o.default_val.dbl, opt.o.default_val.dbl );
				if( opt.o.help )
					curOptInt2D->setHint( opt.o.help );
				break;
			}
			case AV_OPT_TYPE_CONST:
			{
				OFX::ChoiceParamDescriptor* param = static_cast<OFX::ChoiceParamDescriptor*>( desc.getParamDescriptor( opt.class_name + "_" + opt.o.unit + "_preset" ) );
				if( param != 0 )
				{
					param->appendOption( opt.o.name );
					if( opt.o.help )
					{
						OFX::PropertySet& p = param->getProps( );
						std::string help = p.propGetString( kOfxParamPropHint, false );
						if( help.length() )
							help += "\n";
						help += opt.o.name;
						help += ": ";
						help += opt.o.help;
						param->setHint( help );
					}
				}
				else
				{
					OFX::ChoiceParamDescriptor* curOptChoice = desc.defineChoiceParam( opt.class_name + "_" + opt.o.unit + "_preset" );
					curOpt = curOptChoice;
					curOptChoice->setLabel( opt.o.unit );
					curOptChoice->appendOption( opt.o.name );
					curOptChoice->setDefault( 0 );
					if( opt.o.help )
					{
						std::string help = opt.o.name;
						help += ": ";
						help += opt.o.help;
						curOptChoice->setHint( help );
					}
				}
				break;
			}
			default: //< or list all cases to avoid those warnings:
			{
				TUTTLE_CERR( "FFMpeg: options not added : " << opt.o.name << "   " << opt.class_name << " ** " << opt.o.unit << " ** " << opt.o.help );
				//case AV_OPT_TYPE_INT64:
				//case AV_OPT_TYPE_BINARY:
				break;
			}
		}
		curOpt->setParent( groupParams );
	}
}

}

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
	bitDepth->resetOptions();
	bitDepth->appendOption( kTuttlePluginBitDepth8 );
	bitDepth->setDefault( eTuttlePluginBitDepth8 );
	bitDepth->setEnabled( false );

	OFX::BooleanParamDescriptor* premult = static_cast<OFX::BooleanParamDescriptor*>( desc.getParamDescriptor( kParamPremultiplied ) );
	premult->setDefault( true );


	int default_format = 0;
	OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamFormat );
	std::vector<std::string>::const_iterator itLong;
	for( std::vector<std::string>::const_iterator itShort = writer.getFormatsShort().begin(),
		itLong = writer.getFormatsLong().begin(),
		itEnd = writer.getFormatsShort().end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		std::string formatName = *itShort;
		format->appendOption( formatName, *itLong );
		if (!strcmp(itShort->c_str(), "mp4"))
			default_format = format->getNOptions() - 1;
	}
	format->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	format->setDefault( default_format );

	OFX::GroupParamDescriptor* groupFormatParams = desc.defineGroupParam( kParamGroupFormatParams );
	groupFormatParams->setLabel( "Format Advanced Parameters" );
	groupFormatParams->setOpen( false );
	
	declareAVPrivOptions( desc, writer.getFormatPrivOpts(), groupFormatParams );
	
	int default_codec = 0;
	OFX::ChoiceParamDescriptor* codec = desc.defineChoiceParam( kParamCodec );
	for( std::vector<std::string>::const_iterator itShort = writer.getCodecsShort().begin(),
		itLong  = writer.getCodecsLong().begin(),
		itEnd = writer.getCodecsShort().end();
		itShort != itEnd;
		++itShort,
		++itLong )
	{
		std::string codecName = *itShort;
		codec->appendOption( codecName, *itLong );
		if (!strcmp(itShort->c_str(), "mpeg4"))
			default_codec = codec->getNOptions() - 1;
	}
	codec->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	codec->setDefault( default_codec );

	OFX::GroupParamDescriptor* groupCodecParams = desc.defineGroupParam( kParamGroupCodecParams );
	groupCodecParams->setLabel( "Codec Advanced Parameters" );
	groupCodecParams->setOpen( false );
	
	declareAVPrivOptions( desc, writer.getCodecPrivOpts(), groupCodecParams );
	
	OFX::GroupParamDescriptor* groupVideoCodecPreset = desc.defineGroupParam( kParamGroupVideoCodecPreset );
	groupVideoCodecPreset->setLabel( "Codec Video Preset" );
	groupVideoCodecPreset->setOpen( false );
	
	std::vector<std::string> codecListWithPreset = writer.getCodecListWithConfig();
	std::vector<std::string>::iterator it;
	for( it = codecListWithPreset.begin(); it < codecListWithPreset.end(); it++ )
	{
		std::string codecName = *it;
		std::vector<std::string> codecList = writer.getConfigList( codecName );
		OFX::ChoiceParamDescriptor* choiceConfig;
		
		if( codecList.size() != 0 )
		{
			choiceConfig = desc.defineChoiceParam( codecName );
			choiceConfig->setLabel( codecName );
			choiceConfig->setDefault( 0 );
			choiceConfig->setParent( groupVideoCodecPreset );
		}
		
		for( std::vector<std::string>::const_iterator itCodec = codecList.begin(),
			itCodecEnd = codecList.end();
			itCodec != itCodecEnd;
			++itCodec )
		{
			choiceConfig->appendOption( *itCodec );
		}
	}
	
	OFX::IntParamDescriptor* bitrate = desc.defineIntParam( kParamBitrate );
	bitrate->setLabel( "Bitrate" );
	bitrate->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
	bitrate->setDefault( 400000 );

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
