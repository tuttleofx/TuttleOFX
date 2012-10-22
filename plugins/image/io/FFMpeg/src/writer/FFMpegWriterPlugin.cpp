#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterProcess.hpp"
#include "FFMpegWriterDefinitions.hpp"

#include <ffmpeg/VideoFFmpegWriter.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/foreach.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {
/*
void FFMpegWriterPlugin::disableAVOptionsForCodecOrFormat( const std::vector<AVPrivOption>& avPrivOpts, const std::string& codec )
{
	BOOST_FOREACH( const AVPrivOption& opt, avPrivOpts )
	{
		if( opt.o.name == NULL )
			continue;
		
		switch( opt.o.type )
		{
			case AV_OPT_TYPE_FLAGS:
			{
				if( opt.o.name == opt.o.unit )
					continue;
				OFX::BooleanParam* curOpt = fetchBooleanParam( opt.class_name + "_" + opt.o.name );
				curOpt->setIsSecretAndDisabled( strcmp( opt.class_name.c_str(), codec.c_str() ) != 0 );
				break;
			}
			case AV_OPT_TYPE_INT:
			{
				if( opt.o.name == opt.o.unit )
					continue;
				OFX::IntParam* curOpt = fetchIntParam( opt.class_name + "_" + opt.o.name );
				curOpt->setIsSecretAndDisabled( strcmp( opt.class_name.c_str(), codec.c_str() ) != 0 );
				break;
			}
			case AV_OPT_TYPE_FLOAT:
			case AV_OPT_TYPE_DOUBLE:
			{
				if( opt.o.name == opt.o.unit )
					continue;
				OFX::DoubleParam* curOpt = fetchDoubleParam( opt.class_name + "_" + opt.o.name );
				curOpt->setIsSecretAndDisabled( strcmp( opt.class_name.c_str(), codec.c_str() ) != 0 );
				break;
			}
			case AV_OPT_TYPE_STRING:
			{
				if( opt.o.name == opt.o.unit )
					continue;
				OFX::StringParam* curOpt = fetchStringParam( opt.class_name + "_" + opt.o.name );
				curOpt->setIsSecretAndDisabled( strcmp( opt.class_name.c_str(), codec.c_str() ) != 0 );
				break;
			}
			case AV_OPT_TYPE_IMAGE_SIZE:
			case AV_OPT_TYPE_RATIONAL:
			{
				if( opt.o.name == opt.o.unit )
					continue;
				OFX::Int2DParam* curOpt = fetchInt2DParam( opt.class_name + "_" + opt.o.name );
				curOpt->setIsSecretAndDisabled( strcmp( opt.class_name.c_str(), codec.c_str() ) != 0 );
				break;
			}
			case AV_OPT_TYPE_CONST:
			{
				if( opt.o.name == opt.o.unit )
					continue;
				OFX::ChoiceParam* curOpt = fetchChoiceParam( opt.class_name + "_" + opt.o.unit + "_preset" );
				curOpt->setIsSecretAndDisabled( strcmp( opt.class_name.c_str(), codec.c_str() ) != 0 );
				break;
			}
			default: //< or list all cases to avoid those warnings:
			{
				//TUTTLE_CERR( "FFMpeg: options not added : " << opt.o.name << "   " << opt.class_name << " ** " << opt.o.unit << " ** " << opt.o.help );
				//case AV_OPT_TYPE_INT64:
				//case AV_OPT_TYPE_BINARY:
				break;
			}
		}
	}
}*/

FFMpegWriterPlugin::FFMpegWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	// We want to render a sequence
	setSequentialRender( true );

	_paramFormat                      = fetchChoiceParam( kParamFormat );
	_paramVideoCodec                  = fetchChoiceParam( kParamVideoCodec       );
	_paramAudioCodec                  = fetchChoiceParam( kParamAudioCodec       );
	
	_videoCodecListWithPreset = _writer.getCodecListWithConfig();
	std::vector<std::string>::iterator it;
	for( it = _videoCodecListWithPreset.begin(); it < _videoCodecListWithPreset.end(); it++ )
	{
		std::vector<std::string> videoList = _writer.getVideoCodecsShort();
		BOOST_FOREACH( const std::string& videoCodec, videoList )
		{
			if( strcmp( videoCodec.c_str(), (*it).c_str() ) == 0 )
			{
				OFX::ChoiceParam* paramPreset = fetchChoiceParam( *it );
				_videoCodecPresetParams.push_back( paramPreset );
			}
		}

		std::vector<std::string> audioList = _writer.getAudioCodecsShort();
		BOOST_FOREACH( const std::string& audioCodec, audioList )
		{
			if( strcmp( audioCodec.c_str(), (*it).c_str() ) == 0 )
			{
				OFX::ChoiceParam* paramPreset = fetchChoiceParam( *it );
				_audioCodecPresetParams.push_back( paramPreset );
			}
		}
	}
	
	std::string formatName = _writer.getFormatsShort( ).at(_paramFormat->getValue() );
	//disableAVOptionsForCodecOrFormat( _writer.getSpecificFormatPrivOpts(), formatName );
	
	std::string videoCodecName = _writer.getVideoCodecsShort( ).at(_paramVideoCodec->getValue() );
	//disableAVOptionsForCodecOrFormat( _writer.getSpecificVideoCodecPrivOpts(), videoCodecName );
	
	std::string audioCodecName = _writer.getAudioCodecsShort( ).at(_paramAudioCodec->getValue() );
	//disableAVOptionsForCodecOrFormat( _writer.getSpecificAudioCodecPrivOpts(), audioCodecName );
}

FFMpegProcessParams FFMpegWriterPlugin::getProcessParams()
{
	FFMpegProcessParams params;

	params._filepath = _paramFilepath->getValue();
	params._format = _paramFormat->getValue();
	params._videoCodec                     = _paramVideoCodec        ->getValue();
	params._audioCodec                     = _paramAudioCodec        ->getValue();
	
	_writer.setVideoCodec( params._videoCodec );
	const std::string codecName = _writer.getVideoCodec();
	
	params._videoPreset = -1;
	std::vector<OFX::ChoiceParam*>::const_iterator choiceParamIt = _videoCodecPresetParams.begin();
	for( it = _videoCodecListWithPreset.begin(); it < _videoCodecListWithPreset.end(); it++, pos++ )
	{
		if( codecPreset == codecName )
		{
			int presetIdx = _videoCodecPresetParams.at( pos )->getValue();
			params._videoPreset = presetIdx;
		}
		++choiceParamIt;
	}
	return params;
}

void updatePreset()
{
	
}

void FFMpegWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	WriterPlugin::changedParam( args, paramName );
	
	if( paramName == kParamFormat )
	{
		std::string formatName = _writer.getFormatsShort( ).at(_paramFormat->getValue() );
		//disableAVOptionsForCodecOrFormat( _writer.getSpecificFormatPrivOpts(), formatName );
	}
	if( paramName == kParamVideoCodec )
	{
		std::string codecName = _writer.getVideoCodecsShort( ).at(_paramVideoCodec->getValue() );
		//disableAVOptionsForCodecOrFormat( _writer.getSpecificVideoCodecPrivOpts(), codecName );
	}
	if( paramName == kParamAudioCodec )
	{
		std::string codecName = _writer.getAudioCodecsShort( ).at(_paramAudioCodec->getValue() );
		//disableAVOptionsForCodecOrFormat( _writer.getSpecificAudioCodecPrivOpts(), codecName );
	}
	/*
	if( paramName == kParamVideoCodecPreset )
	{
		std::string codecName = _writer.getVideoCodecsShort( ).at(_paramVideoCodec->getValue() );
		std::vector<std::string>::iterator it;
		size_t pos = 0;
		for( it = _codecListWithPreset.begin(); it < _codecListWithPreset.end(); it++, pos++ )
		{
			if( strcmp( codecName.c_str(), (*it).c_str() ) == 0 )
			{
				int presetIdx = videoCodecPresets.at( pos )->getValue();
				params._videoPreset = presetIdx;
				std::string presetFilename = _writer.getFilename( codecName, presetIdx );
			}
		}
		s
		PresetsOptions opts = getOptionsForPresetFilename( presetFilename );
		PresetsOptions::iterator itOpt;
		for ( itOpt = opts.begin() ; itOpt != opts.end(); itOpt++ )
		{
			int ret = av_opt_set( (void*)_stream->codec, (*itOpt).first.c_str(), (*itOpt).second.c_str(), 0);
			switch( ret )
			{
				case AVERROR_OPTION_NOT_FOUND: TUTTLE_CERR( "ffmpegPreset: unable to find " << (*itOpt).first ); break;
				case AVERROR(EINVAL): TUTTLE_CERR( "ffmpegPreset: invalid value " << (*itOpt).second.c_str() << " for option " << (*itOpt).first ); break;
				case AVERROR(ERANGE): TUTTLE_CERR( "ffmpegPreset: invalid range for parameter " << (*itOpt).first << " : " << (*itOpt).second.c_str() ); break;
			}
		}
	}*/
}

void FFMpegWriterPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	// If pattern detected (frame varying on time)
	clipPreferences.setOutputFrameVarying( true );
}

bool FFMpegWriterPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, OfxTime& identityTime )
{
	return WriterPlugin::isIdentity( args, identityClip, identityTime );
}

void FFMpegWriterPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	WriterPlugin::beginSequenceRender( args );

	FFMpegProcessParams params = getProcessParams();

	_writer.setFilename    ( params._filepath );
	_writer.setFormat      ( params._format );
	_writer.setVideoCodec  ( params._videoCodec );
	_writer.setFps         ( _clipSrc->getFrameRate() );
	_writer.setAspectRatio ( _clipSrc->getPixelAspectRatio() );
	_writer.setVideoPreset ( params._videoPreset );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FFMpegWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );
	
	doGilRender<FFMpegWriterProcess>( *this, args );
}

void FFMpegWriterPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{
	_writer.finish();
}

}
}
}
}
