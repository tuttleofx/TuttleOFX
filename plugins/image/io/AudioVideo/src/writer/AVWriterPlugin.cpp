#include "AVWriterPlugin.hpp"
#include "AVWriterProcess.hpp"
#include "AVWriterDefinitions.hpp"

#include <libav/LibAVPreset.hpp>
#include <libav/LibAVFormatPreset.hpp>
#include <libav/LibAVVideoPreset.hpp>
#include <libav/LibAVAudioPreset.hpp>
#include <libav/LibAVVideoWriter.hpp>
#include <libav/LibAVOptions.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/foreach.hpp>

#include <cctype>

namespace tuttle {
namespace plugin {
namespace av {
namespace writer {

void AVWriterPlugin::disableAVOptionsForCodecOrFormat( const std::vector<AVPrivOption>& avPrivOpts, const std::string& codec )
{
	std::vector<OFX::GroupParam*> groups;

	BOOST_FOREACH( AVPrivOption opt, avPrivOpts )
	{
		if( opt.o.unit && opt.o.type == AV_OPT_TYPE_FLAGS )
		{
			std::string name = "g_";
			name += opt.class_name;
			name += "_";
			name += opt.o.unit;
			
			OFX::GroupParam* curOpt = fetchGroupParam( name );
			curOpt->setIsSecretAndDisabled( !( opt.class_name == codec ) );
			
			groups.push_back( curOpt );
			continue;
		}
		if( opt.o.unit && opt.o.type == AV_OPT_TYPE_INT )
		{
			std::string name = opt.class_name;
			name += "_";
			name += opt.o.name;
			
			OFX::ChoiceParam* curOpt = fetchChoiceParam( name );
			curOpt->setIsSecretAndDisabled( !( opt.class_name == codec ) );
			continue;
		}
		
		std::string name = opt.class_name;
		name += "_";
		name += opt.o.name;
		
		switch( opt.o.type )
		{
			case AV_OPT_TYPE_FLAGS:
			{
				OFX::BooleanParam* curOpt = fetchBooleanParam( name );
				curOpt->setIsSecretAndDisabled( !( opt.class_name == codec ) );
				break;
			}
			case AV_OPT_TYPE_INT:
			case AV_OPT_TYPE_INT64:
			{
				OFX::IntParam* curOpt = fetchIntParam( name );
				curOpt->setIsSecretAndDisabled( !( opt.class_name == codec ) );
				break;
			}
			case AV_OPT_TYPE_DOUBLE:
			case AV_OPT_TYPE_FLOAT:
			{
				OFX::DoubleParam* curOpt = fetchDoubleParam( name );
				curOpt->setIsSecretAndDisabled( !( opt.class_name == codec ) );
				break;
			}
			case AV_OPT_TYPE_STRING:
			{
				OFX::StringParam* curOpt = fetchStringParam( name );
				curOpt->setIsSecretAndDisabled( !( opt.class_name == codec ) );
				break;
			}
			case AV_OPT_TYPE_RATIONAL:
			{
				OFX::Int2DParam* curOpt = fetchInt2DParam( name );
				curOpt->setIsSecretAndDisabled( !( opt.class_name == codec ) );
				break;
			}
			case AV_OPT_TYPE_BINARY:
			{
				OFX::StringParam* curOpt = fetchStringParam( name );
				curOpt->setIsSecretAndDisabled( !( opt.class_name == codec ) );
				break;
			}
			case AV_OPT_TYPE_CONST:
			{
				break;
			}
			default:
			{
				TUTTLE_LOG_WARNING( "AudioVideo: undefined type for " << opt.o.name );
			}
		}
	}
	
	BOOST_FOREACH( AVPrivOption opt, avPrivOpts )
	{
		switch( opt.o.type )
		{
			case AV_OPT_TYPE_CONST:
			{
				BOOST_FOREACH( OFX::GroupParam* g, groups )
				{
					std::string name = "g_";
					name += opt.class_name;
					name += "_";
					name += opt.o.unit;
					if( name == g->getName() )
					{
						std::string name = "flags_";
						name += opt.class_name;
						name += "_";
						name += opt.o.name;
						
						OFX::BooleanParam* curOpt = fetchBooleanParam( name );
						curOpt->setIsSecretAndDisabled( !( opt.class_name == codec ) );
						break;
					}
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

void AVWriterPlugin::updatePixelFormat( const std::string& videoCodecName )
{
	AVCodec* _videoCodec = avcodec_find_encoder_by_name( videoCodecName.c_str() );
	
	_paramVideoPixelFormat->resetOptions();
	if( _videoCodec->pix_fmts == 0 )
	{
		for( int pix_fmt = 0; pix_fmt < PIX_FMT_NB; pix_fmt++ )
		{
			const AVPixFmtDescriptor *pix_desc = &av_pix_fmt_descriptors[pix_fmt];
			if(!pix_desc->name)
				continue;
			_paramVideoPixelFormat->appendOption( pix_desc->name );
		}
		return;
	}
	
	int i = 0;
	while( _videoCodec->pix_fmts[i] != -1 )
	{
		const AVPixFmtDescriptor *pix_desc = &av_pix_fmt_descriptors[ _videoCodec->pix_fmts[i] ];
		_paramVideoPixelFormat->appendOption( pix_desc->name );
		i++;
	}
}

AVWriterPlugin::AVWriterPlugin( OfxImageEffectHandle handle )
	: AVOptionPlugin( handle )
	, _initWriter ( false )
{
	// We want to render a sequence
	setSequentialRender( true );

	_paramFormat     = AVOptionPlugin::fetchChoiceParam( kParamFormat );
	_paramVideoCodec = fetchChoiceParam( kParamVideoCodec );
	_paramAudioCodec = fetchChoiceParam( kParamAudioCodec );
	
	_paramMainPreset       = fetchChoiceParam( kParamMainPreset );
	_paramFormatPreset     = fetchChoiceParam( kParamFormatPreset );
	_paramVideoCodecPreset = fetchChoiceParam( kParamVideoPreset );
	_paramAudioCodecPreset = fetchChoiceParam( kParamAudioPreset );
	
	_paramUseCustomFps     = fetchBooleanParam( kParamUseCustomFps );
	_paramCustomFps        = fetchDoubleParam( kParamCustomFps );
	
	_paramVideoPixelFormat = fetchChoiceParam( kParamVideoCodecPixelFmt );
	
	std::string formatName = _writer.getFormatsShort( ).at(_paramFormat->getValue() );
	disableAVOptionsForCodecOrFormat( _writer.getFormatPrivOpts(), formatName );
	
	std::string videoCodecName = _writer.getVideoCodecsShort( ).at(_paramVideoCodec->getValue() );
	disableAVOptionsForCodecOrFormat( _writer.getVideoCodecPrivOpts(), videoCodecName );
	
	updatePixelFormat( videoCodecName );
	
	std::string audioCodecName = _writer.getAudioCodecsShort( ).at(_paramAudioCodec->getValue() );
	disableAVOptionsForCodecOrFormat( _writer.getAudioCodecPrivOpts(), audioCodecName );
}

AVProcessParams AVWriterPlugin::getProcessParams()
{
	AVProcessParams params;

	params._filepath = _paramFilepath->getValue();
	params._format = _paramFormat->getValue();
	params._videoCodec                     = _paramVideoCodec        ->getValue();
	params._audioCodec                     = _paramAudioCodec        ->getValue();
	params._videoPixelFormat               = static_cast<PixelFormat>( _paramVideoPixelFormat->getValue() );
	
	_writer.setVideoCodec( params._videoCodec );

	return params;
}

void AVWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	WriterPlugin::changedParam( args, paramName );
	
	if( paramName == kParamFormat )
	{
		std::string formatName = _writer.getFormatsShort( ).at(_paramFormat->getValue() );
		disableAVOptionsForCodecOrFormat( _writer.getFormatPrivOpts(), formatName );
	}
	if( paramName == kParamVideoCodec )
	{
		std::string videoCodecName = _writer.getVideoCodecsShort( ).at(_paramVideoCodec->getValue() );
		disableAVOptionsForCodecOrFormat( _writer.getVideoCodecPrivOpts(), videoCodecName );
		updatePixelFormat( videoCodecName );
	}
	if( paramName == kParamAudioCodec )
	{
		std::string codecName = _writer.getAudioCodecsShort( ).at(_paramAudioCodec->getValue() );
		disableAVOptionsForCodecOrFormat( _writer.getAudioCodecPrivOpts(), codecName );
	}
	
	if( paramName == kParamMainPreset )
	{
		//TUTTLE_LOG_TRACE( "preset change " << _paramMainPreset->getValue() );
		if( _paramMainPreset->getValue() == 0 )
			return;
		
		std::vector<std::string> idList;
		std::vector<std::string> idLabelList;
		LibAVPreset::getPresetList( idList, idLabelList );
		
		LibAVPreset p( idList.at( _paramMainPreset->getValue() - 1 ) );
		
		std::vector<std::string> idFormatList;
		LibAVFormatPreset::getPresetList( idFormatList );

		std::vector<std::string> idVideoList;
		LibAVVideoPreset::getPresetList( idVideoList );
		
		std::vector<std::string> idAudioList;
		LibAVAudioPreset::getPresetList( idAudioList);
		
		int formatIndex = 0;
		int videoIndex  = 0;
		int audioIndex  = 0;
		for( unsigned int id = 0; id < idFormatList.size(); id++ )
		{
			if( idFormatList.at( id ) == p.getFormatID() )
			{
				formatIndex = id + 1;
				break;
			}
		}
		
		for( unsigned int id = 0; id < idVideoList.size(); id++ )
		{
			if( idVideoList.at( id ) == p.getVideoID() )
			{
				videoIndex = id + 1;
				break;
			}
		}
		
		
		for( unsigned int id = 0; id < idAudioList.size(); id++ )
		{
			if( idAudioList.at( id ) == p.getAudioID() )
			{
				audioIndex = id + 1;
				break;
			}
		}
		/*
		TUTTLE_LOG_INFO( "set format at " << formatIndex );
		TUTTLE_LOG_INFO( "set video at " << formatIndex );
		TUTTLE_LOG_INFO( "set audio at " << formatIndex );*/
		
		_paramFormatPreset->setValue( formatIndex );
		_paramVideoCodecPreset->setValue( videoIndex );
		_paramAudioCodecPreset->setValue( audioIndex );
	}
	
	if( paramName == kParamFormatPreset )
	{
		//TUTTLE_LOG_INFO( "preset change " << _paramFormatPreset->getValue() );
		if( _paramFormatPreset->getValue() == 0 )
			return;
		std::vector<std::string> idFormatList;
		LibAVFormatPreset::getPresetList( idFormatList );
		
		LibAVFormatPreset p( idFormatList.at( _paramFormatPreset->getValue() - 1 ) );
		setParameters( p.getParameters() );
	}
	
	if( paramName == kParamVideoPreset )
	{
		if( _paramVideoCodecPreset->getValue() == 0 )
			return;
		std::vector<std::string> idVideoList;
		LibAVVideoPreset::getPresetList( idVideoList );
		
		LibAVVideoPreset p( idVideoList.at( _paramVideoCodecPreset->getValue() - 1 ) );
		setParameters( p.getParameters() );
	}
	
	if( paramName == kParamAudioPreset )
	{
		if( _paramAudioCodecPreset->getValue() == 0 )
			return;
		std::vector<std::string> idAudioList;
		LibAVAudioPreset::getPresetList( idAudioList);
		
		LibAVAudioPreset p( idAudioList.at( _paramAudioCodecPreset->getValue() - 1 ) );
		setParameters( p.getParameters() );
	}
}

void AVWriterPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	// Need to be computed at each frame
	clipPreferences.setOutputFrameVarying( true );
}

bool AVWriterPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, OfxTime& identityTime )
{
	return WriterPlugin::isIdentity( args, identityClip, identityTime );
}

void AVWriterPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	_initWriter = false;
	WriterPlugin::beginSequenceRender( args );
	
	AVProcessParams params = getProcessParams();
	
	//TUTTLE_LOG_VAR( TUTTLE_WARNING, _clipSrc->getFrameRate() );
	
	_writer.setFilename    ( params._filepath );
	_writer.setFormat      ( params._format );
	_writer.setVideoCodec  ( params._videoCodec );
	if( _paramUseCustomFps->getValue() )
	{
		_writer.setFps( _paramCustomFps->getValue() );
	}
	else
	{
		_writer.setFps( _clipSrc->getFrameRate() );
	}
	_writer.setAspectRatio ( _clipSrc->getPixelAspectRatio() );
	_writer.setPixelFormat ( params._videoPixelFormat );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void AVWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );
	
	//OfxRangeD range = args.frameRange;
	const OfxRectI bounds = _clipSrc->getPixelRod( args.time, args.renderScale );
	_writer.setWidth ( bounds.x2 - bounds.x1 );
	_writer.setHeight( bounds.y2 - bounds.y1 );
	
	if( !_initWriter )
	{
		_writer.start( );
	
		// set Format parameters
		AVFormatContext* avFormatContext;
		avFormatContext = avformat_alloc_context();
		setParameters( _writer, eAVParamFormat, (void*)avFormatContext, AV_OPT_FLAG_ENCODING_PARAM, 0 );
		
		std::string formatName = _writer.getFormatsShort( ).at(_paramFormat->getValue() );
		setParameters( _writer, eAVParamFormat, _writer.getFormatPrivOpts(), formatName );
		
		// set Video Codec parameters
		AVCodecContext* avCodecContext;
		
	#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT( 53, 8, 0 )
		avCodecContext = avcodec_alloc_context();
		// deprecated in the same version
		//avCodecContext = avcodec_alloc_context2( AVMEDIA_TYPE_UNKNOWN );
	#else
		AVCodec* avCodec = NULL;
		avCodecContext = avcodec_alloc_context3( avCodec );
	#endif
		setParameters( _writer, eAVParamVideo, (void*)avCodecContext, AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM, 0 );
		
		std::string codecName = _writer.getVideoCodecsShort( ).at(_paramVideoCodec->getValue() );
		setParameters( _writer, eAVParamVideo, _writer.getVideoCodecPrivOpts(), codecName );
		
		// set Audio Codec parameters
		//codecName = _writer.getAudioCodecsShort( ).at(_paramAudioCodec->getValue() );
		//setParameters( _writer, eAVParamAudio, _writer.getAudioCodecPrivOpts(), codecName );
		
		_writer.finishInit();
		
		_initWriter = true;
	}
	
	doGilRender<AVWriterProcess>( *this, args );
}

void AVWriterPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{
	_writer.finish();
	_initWriter = false;
}

}
}
}
}
