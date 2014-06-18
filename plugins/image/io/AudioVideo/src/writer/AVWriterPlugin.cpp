#include "AVWriterPlugin.hpp"
#include "AVWriterProcess.hpp"
#include "AVWriterDefinitions.hpp"

#include <libav/LibAVPreset.hpp>
#include <libav/LibAVFormatPreset.hpp>
#include <libav/LibAVVideoPreset.hpp>
#include <libav/LibAVAudioPreset.hpp>

#include <AvTranscoder/DatasStructures/AudioDesc.hpp>
#include <AvTranscoder/DatasStructures/VideoDesc.hpp>

extern "C" {
#ifndef __STDC_CONSTANT_MACROS
	#define __STDC_CONSTANT_MACROS
#endif
#include <libavutil/pixdesc.h>
}

#include <boost/gil/gil_all.hpp>
#include <boost/foreach.hpp>

#include <cctype>

namespace tuttle {
namespace plugin {
namespace av {
namespace writer {

void AVWriterPlugin::disableAVOptionsForCodecOrFormat( avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& codec )
{
	// iterate on map keys
	BOOST_FOREACH( avtranscoder::OptionLoader::OptionMap::value_type& subGroupOption, optionsMap )
	{
		std::string subGroupName = subGroupOption.first;
		std::vector<avtranscoder::Option>& options = subGroupOption.second;
				
		// iterate on options
		BOOST_FOREACH( avtranscoder::Option& option, options )
		{
			std::string name = subGroupName;
			name += "_";
			name += option.getName();
			
			switch( option.getType() )
			{
				case avtranscoder::TypeBool:
				{
					OFX::BooleanParam* curOpt = fetchBooleanParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					break;
				}
				case avtranscoder::TypeInt:
				{
					OFX::IntParam* curOpt = fetchIntParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					break;
				}
				case avtranscoder::TypeDouble:
				{
					OFX::DoubleParam* curOpt = fetchDoubleParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					break;
				}
				case avtranscoder::TypeString:
				{
					OFX::StringParam* curOpt = fetchStringParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					break;
				}
				case avtranscoder::TypeRatio:
				{
					OFX::Int2DParam* curOpt = fetchInt2DParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					break;
				}
				case avtranscoder::TypeChoice:
				{
					OFX::ChoiceParam* curOpt = fetchChoiceParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					break;
				}
				case avtranscoder::TypeGroup:
				{
					std::string groupName = "g_";
					groupName += subGroupName;
					groupName += "_";
					groupName += option.getName();
					
					OFX::GroupParam* curOpt = fetchGroupParam( groupName );
					curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					
					BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
					{
						std::string ChildName = "flags_";
						ChildName += subGroupName;
						ChildName += "_";
						ChildName += child.getName();
						
						OFX::BooleanParam* curOpt = fetchBooleanParam( ChildName );
						curOpt->setIsSecretAndDisabled( !( subGroupName == codec ) );
					}
					break;
				}
				default:
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
	: WriterPlugin( handle )
	, _outputFile( NULL )
	, _outputStreamVideo( NULL )
	, _initWriter( false )
	, _lastOutputFilePath( "" )
{
	// We want to render a sequence
	setSequentialRender( true );

	_paramFormat     = fetchChoiceParam( kParamFormat );
	_paramVideoCodec = fetchChoiceParam( kParamVideoCodec );
	_paramAudioCodec = fetchChoiceParam( kParamAudioCodec );
	
	_paramMainPreset       = fetchChoiceParam( kParamMainPreset );
	_paramFormatPreset     = fetchChoiceParam( kParamFormatPreset );
	_paramVideoCodecPreset = fetchChoiceParam( kParamVideoPreset );
	_paramAudioCodecPreset = fetchChoiceParam( kParamAudioPreset );
	
	_paramUseCustomFps     = fetchBooleanParam( kParamUseCustomFps );
	_paramCustomFps        = fetchDoubleParam( kParamCustomFps );
	
	_paramVideoPixelFormat = fetchChoiceParam( kParamVideoCodecPixelFmt );
	
	avtranscoder::OptionLoader::OptionMap optionsFormatMap = _optionLoader.loadOutputFormatOptions();
	const std::string formatName = _optionLoader.getFormatsShortNames().at( _paramFormat->getValue() );
	disableAVOptionsForCodecOrFormat( optionsFormatMap, formatName );
	
	avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
	const std::string videoCodecName = _optionLoader.getVideoCodecsShortNames().at(_paramVideoCodec->getValue() );
	disableAVOptionsForCodecOrFormat( optionsVideoCodecMap, videoCodecName );
	
	updatePixelFormat( videoCodecName );
	
	avtranscoder::OptionLoader::OptionMap optionsAudioCodecMap = _optionLoader.loadAudioCodecOptions();
	const std::string audioCodecName = _optionLoader.getAudioCodecsShortNames().at(_paramAudioCodec->getValue() );
	disableAVOptionsForCodecOrFormat( optionsAudioCodecMap, audioCodecName );
	
	_paramMetadatas.push_back( fetchStringParam( kParamMetaAlbum           ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaAlbumArtist     ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaArtist          ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaComment         ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaComposer        ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaCopyright       ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaCreationTime    ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaDate            ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaDisc            ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaEncoder         ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaEncodedBy       ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaFilename        ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaGenre           ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaLanguage        ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaPerformer       ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaPublisher       ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaServiceName     ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaServiceProvider ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaTitle           ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaTrack           ) );
	_paramMetadatas.push_back( fetchStringParam( kParamMetaVariantBitrate  ) );

	updateVisibleTools();
}

void AVWriterPlugin::updateVisibleTools()
{
	OFX::InstanceChangedArgs args( this->timeLineGetTime() );
	changedParam( args, kParamUseCustomFps );
}

AVProcessParams AVWriterPlugin::getProcessParams()
{
	AVProcessParams params;

	params._filepath = _paramFilepath->getValue();
	
	params._format = _paramFormat->getValue();
	params._formatName = _optionLoader.getFormatsShortNames().at( params._format );
	
	params._videoCodec = _paramVideoCodec->getValue();
	params._videoCodecName = _optionLoader.getVideoCodecsShortNames().at( params._videoCodec );
	
	params._audioCodec = _paramAudioCodec->getValue();
	params._audioCodecName = _optionLoader.getAudioCodecsShortNames().at( params._audioCodec );
	
	// av_get_pix_fmt( _paramVideoPixelFormat->getValue() )
	params._videoPixelFormat = static_cast<AVPixelFormat>( _paramVideoPixelFormat->getValue() );
	
	BOOST_FOREACH( OFX::StringParam* parameter, _paramMetadatas )
	{
		if( parameter->getValue().size() > 0 )
		{
			std::string ffmpegKey = parameter->getName();
			ffmpegKey.erase( 0, 5 );
			params._metadatas[ ffmpegKey ] = parameter->getValue();
		}
	}
	return params;
}

void AVWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	WriterPlugin::changedParam( args, paramName );
	
	if( paramName == kParamFormat )
	{
		avtranscoder::OptionLoader::OptionMap optionsFormatMap = _optionLoader.loadOutputFormatOptions();
		const std::string formatName = _optionLoader.getFormatsShortNames().at( _paramFormat->getValue() );
		disableAVOptionsForCodecOrFormat( optionsFormatMap, formatName );
	}
	else if( paramName == kParamVideoCodec )
	{
		avtranscoder::OptionLoader::OptionMap optionsVideoCodecMap = _optionLoader.loadVideoCodecOptions();
		const std::string videoCodecName = _optionLoader.getVideoCodecsShortNames().at(_paramVideoCodec->getValue() );
		disableAVOptionsForCodecOrFormat( optionsVideoCodecMap, videoCodecName );
		updatePixelFormat( videoCodecName );
	}
	else if( paramName == kParamAudioCodec )
	{
		avtranscoder::OptionLoader::OptionMap optionsAudioCodecMap = _optionLoader.loadAudioCodecOptions();
		const std::string audioCodecName = _optionLoader.getAudioCodecsShortNames().at(_paramAudioCodec->getValue() );
		disableAVOptionsForCodecOrFormat( optionsAudioCodecMap, audioCodecName );
	}
	else if( paramName == kParamMainPreset )
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
	else if( paramName == kParamFormatPreset )
	{
		//TUTTLE_LOG_INFO( "preset change " << _paramFormatPreset->getValue() );
		if( _paramFormatPreset->getValue() == 0 )
			return;
		std::vector<std::string> idFormatList;
		LibAVFormatPreset::getPresetList( idFormatList );
		
		//LibAVFormatPreset p( idFormatList.at( _paramFormatPreset->getValue() - 1 ) );
		//setParameters( p.getParameters() );
	}
	else if( paramName == kParamVideoPreset )
	{
		if( _paramVideoCodecPreset->getValue() == 0 )
			return;
		std::vector<std::string> idVideoList;
		LibAVVideoPreset::getPresetList( idVideoList );
		
		//LibAVVideoPreset p( idVideoList.at( _paramVideoCodecPreset->getValue() - 1 ) );
		//setParameters( p.getParameters() );
	}
	else if( paramName == kParamAudioPreset )
	{
		if( _paramAudioCodecPreset->getValue() == 0 )
			return;
		std::vector<std::string> idAudioList;
		LibAVAudioPreset::getPresetList( idAudioList);
		
		//LibAVAudioPreset p( idAudioList.at( _paramAudioCodecPreset->getValue() - 1 ) );
		//setParameters( p.getParameters() );
	}
	else if( paramName == kParamUseCustomFps )
	{
		_paramCustomFps->setIsSecretAndDisabled( ! _paramUseCustomFps->getValue() );
	}
	else if( paramName == kParamCustomFps )
	{
		if( ! _paramUseCustomFps->getValue() )
		{
			_paramUseCustomFps->setValue(true);
		}
	}
}

void AVWriterPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	// Need to be computed at each frame
	clipPreferences.setOutputFrameVarying( true );
}

void AVWriterPlugin::ensureWriterIsInit( const OFX::RenderArguments& args )
{
	AVProcessParams params = getProcessParams();
	
	if( _lastOutputFilePath == params._filepath && // path already set
		! _lastOutputFilePath.empty() ) // not the first time...
		return;
	
	// create output file
	try
	{
		_outputFile.reset( new avtranscoder::OutputFile( params._filepath ) );
		_lastOutputFilePath = params._filepath;
		_outputFile->setup(); //guess format from filename
	}
	catch( std::exception& e )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to open output file : " + e.what()
		    << exception::filename( params._filepath ) );
	}
	
	// create video stream
	try
	{
		_outputStreamVideo.reset( new avtranscoder::OutputStreamVideo() );

		avtranscoder::VideoDesc& videoOutputDesc = _outputStreamVideo->getVideoDesc(); 
		videoOutputDesc.setVideoCodec( params._videoCodecName );

		//OfxRangeD range = args.frameRange;
		const OfxRectI bounds = _clipSrc->getPixelRod( args.time, args.renderScale );
		int width = bounds.x2 - bounds.x1;
		int height = bounds.y2 - bounds.y1;

		avtranscoder::ImageDesc imageDesc;
		avtranscoder::Pixel pixel( params._videoPixelFormat );
		imageDesc.setPixel( pixel );
		imageDesc.setWidth( width );
		imageDesc.setHeight( height );
		imageDesc.setDar( _clipSrc->getPixelAspectRatio(), 1 );
		videoOutputDesc.setImageParameters( imageDesc );

		if( _paramUseCustomFps->getValue() )
		{
			videoOutputDesc.setTimeBase( 1, _paramCustomFps->getValue() );
		}
		else
		{
			videoOutputDesc.setTimeBase( 1, _clipSrc->getFrameRate() );
		}

		if( ! _outputStreamVideo->setup( ) )
		{
			throw std::runtime_error( "error during initialising video output stream" );
		}
		
		_outputFile->addVideoStream( videoOutputDesc );
	}
	catch( std::exception& e )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "unable to create video stream : " + e.what() );
	}
	
	_outputFile->beginWrap();
	
	_initWriter = true;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void AVWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );
	
	ensureWriterIsInit( args );
	
	doGilRender<AVWriterProcess>( *this, args );
}

void AVWriterPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{	
	if( ! _initWriter )
		return;
	
	avtranscoder::DataStream codedImage;
	
	// if latency
	while( _outputStreamVideo->encodeFrame( codedImage ) )
	{
		_outputFile->wrap( codedImage, 0 );
	}
	
	_outputFile->endWrap();
}

}
}
}
}
