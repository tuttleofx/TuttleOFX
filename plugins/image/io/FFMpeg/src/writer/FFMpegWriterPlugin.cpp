#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterProcess.hpp"
#include "FFMpegWriterDefinitions.hpp"

#include <ffmpeg/LibAVPreset.hpp>
#include <ffmpeg/LibAVFormatPreset.hpp>
#include <ffmpeg/LibAVVideoPreset.hpp>
#include <ffmpeg/LibAVAudioPreset.hpp>
#include <ffmpeg/VideoFFmpegWriter.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <cctype>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

void FFMpegWriterPlugin::disableAVOptionsForCodecOrFormat( const std::vector<AVPrivOption>& avPrivOpts, const std::string& codec )
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
			case AV_OPT_TYPE_PIXEL_FMT:
			{
				TUTTLE_COUT( "unsupported pixel format parameter" );
				break;
			}
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT( 52, 3, 100 )
			case AV_OPT_TYPE_SAMPLE_FMT:
			{
                                TUTTLE_COUT( "unsupported sample format parameter" );
                                break;
                        }
#endif
			case AV_OPT_TYPE_IMAGE_SIZE:
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

void FFMpegWriterPlugin::updatePixelFormat( const std::string& videoCodecName )
{
	AVCodec* _videoCodec = avcodec_find_encoder_by_name( videoCodecName.c_str() );
	
	_paramVideoPixelFormat->resetOptions();
	
	int i = 0;
	while( _videoCodec->pix_fmts[i] != -1 )
	{
		const AVPixFmtDescriptor *pix_desc = &av_pix_fmt_descriptors[ _videoCodec->pix_fmts[i] ];
		_paramVideoPixelFormat->appendOption( pix_desc->name );
		i++;
	}
}

FFMpegWriterPlugin::FFMpegWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
	, _initWriter ( false )
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
	
	_paramVideoPixelFormat = fetchChoiceParam( kParamVideoCodecPixelFmt );
	
	std::string formatName = _writer.getFormatsShort( ).at(_paramFormat->getValue() );
	disableAVOptionsForCodecOrFormat( _writer.getFormatPrivOpts(), formatName );
	
	std::string videoCodecName = _writer.getVideoCodecsShort( ).at(_paramVideoCodec->getValue() );
	disableAVOptionsForCodecOrFormat( _writer.getVideoCodecPrivOpts(), videoCodecName );
	
	updatePixelFormat( videoCodecName );
	
	std::string audioCodecName = _writer.getAudioCodecsShort( ).at(_paramAudioCodec->getValue() );
	disableAVOptionsForCodecOrFormat( _writer.getAudioCodecPrivOpts(), audioCodecName );
}

FFMpegProcessParams FFMpegWriterPlugin::getProcessParams()
{
	FFMpegProcessParams params;

	params._filepath = _paramFilepath->getValue();
	params._format = _paramFormat->getValue();
	params._videoCodec                     = _paramVideoCodec        ->getValue();
	params._audioCodec                     = _paramAudioCodec        ->getValue();
	params._videoPixelFormat               = static_cast<PixelFormat>( _paramVideoPixelFormat->getValue() );
	
	_writer.setVideoCodec( params._videoCodec );

	return params;
}

void FFMpegWriterPlugin::setParameters( const EAVParamType& type, void* av_class, int req_flags, int rej_flags )
{
	std::vector<OFX::GroupParam*> groups;
	const AVOption *opt = NULL;

	while( (opt = av_opt_next( av_class, opt)  ) != NULL )
	{
		if( !opt ||
			!opt->name ||
			(opt->flags & req_flags) != req_flags ||
			(opt->flags & rej_flags) )
		{
			continue;
		}
		
		if( opt->unit && opt->type == AV_OPT_TYPE_FLAGS )
		{
			std::string name = "g_";
			name += opt->unit;
			OFX::GroupParam* curOpt = fetchGroupParam( name );
			groups.push_back( curOpt );
			continue;
		}
		if( opt->unit && opt->type == AV_OPT_TYPE_INT )
		{
			OFX::ChoiceParam* curOpt = fetchChoiceParam( opt->name );
			int v = curOpt->getValue();
			_writer.optionSet( type, *opt, v );
			continue;
		}
		
		switch( opt->type )
		{
			case AV_OPT_TYPE_FLAGS:
			{
				OFX::BooleanParam* curOpt = fetchBooleanParam( opt->name );
				bool v = curOpt->getValue();
				_writer.optionSet( type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_INT:
			case AV_OPT_TYPE_INT64:
			{
				OFX::IntParam* curOpt = fetchIntParam( opt->name );
				int v = curOpt->getValue();
				_writer.optionSet( type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_DOUBLE:
			case AV_OPT_TYPE_FLOAT:
			{
				OFX::DoubleParam* curOpt = fetchDoubleParam( opt->name );
				double v = curOpt->getValue();
				_writer.optionSet( type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_STRING:
			{
				OFX::StringParam* curOpt = fetchStringParam( opt->name );
				std::string v = curOpt->getValue();
				_writer.optionSet( type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_RATIONAL:
			{
				OFX::Int2DParam* curOpt = fetchInt2DParam( opt->name );
				int vn = curOpt->getValue().x;
				int vd = curOpt->getValue().y;
				_writer.optionSet( type, *opt, vn, vd );
				break;
			}
			case AV_OPT_TYPE_IMAGE_SIZE:
			{
				OFX::Int2DParam* curOpt = fetchInt2DParam( opt->name );
				int vn = curOpt->getValue().x;
				int vd = curOpt->getValue().y;
				_writer.optionSetImageSize( type, *opt, vn, vd );
				break;
			}
			case AV_OPT_TYPE_PIXEL_FMT:
			{
				TUTTLE_COUT( "add pixel fmt " << opt->name );
			}
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT( 52, 3, 100 )
                        case AV_OPT_TYPE_SAMPLE_FMT:
                        {
                                TUTTLE_COUT( "unsupported sample format parameter" );
                                break;
                        }
#endif
			case AV_OPT_TYPE_BINARY:
			{
				OFX::StringParam* curOpt = fetchStringParam( opt->name );
				std::string v = curOpt->getValue();
				_writer.optionSet( type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_CONST:
			{
				break;
			}
		}
	}
	
	// adding enum values and flags in groups
	opt = NULL;
	while( (opt = av_opt_next( av_class, opt)  ) != NULL )
	{
		if( !opt ||
			!opt->name ||
			( opt->flags & req_flags ) != req_flags ||
			( opt->flags & rej_flags ) ||
			( opt->unit && opt->type == AV_OPT_TYPE_FLAGS ) ||
			( opt->unit && opt->type == AV_OPT_TYPE_INT ) )
		{
			continue;
		}
		
		switch( opt->type )
		{
			case AV_OPT_TYPE_CONST:
			{
				BOOST_FOREACH( OFX::GroupParam* g, groups )
				{
					std::string name = "g_";
					name += opt->unit;
					if( name == g->getName() )
					{
						OFX::BooleanParam* curOpt = fetchBooleanParam( opt->name );
						bool v = curOpt->getValue();
						std::string optName = opt->unit;
						_writer.optionSet( type, *opt, v, optName );
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

void FFMpegWriterPlugin::setParameters( const EAVParamType& type, const std::vector<AVPrivOption>& avPrivOpts, const std::string& codec )
{
	std::vector<OFX::GroupParam*> groups;

	BOOST_FOREACH( AVPrivOption opt, avPrivOpts )
	{
		if( opt.class_name != codec )
			continue;
		
		if( opt.o.unit && opt.o.type == AV_OPT_TYPE_FLAGS )
		{
			std::string name = "g_";
			name += opt.class_name;
			name += "_";
			name += opt.o.unit;
			
			OFX::GroupParam* curOpt = fetchGroupParam( name );
			groups.push_back( curOpt );
			continue;
		}
		if( opt.o.unit && opt.o.type == AV_OPT_TYPE_INT )
		{
			std::string name = opt.class_name;
			name += "_";
			name += opt.o.name;
			
			OFX::ChoiceParam* curOpt = fetchChoiceParam( name );
			int v = curOpt->getValue();
			_writer.optionSet( type, opt.o, v );
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
				bool v = curOpt->getValue();
				_writer.optionSet( type, opt.o, v );
				break;
			}
			case AV_OPT_TYPE_INT:
			case AV_OPT_TYPE_INT64:
			{
				OFX::IntParam* curOpt = fetchIntParam( name );
				int v = curOpt->getValue();
				_writer.optionSet( type, opt.o, v );
				break;
			}
			case AV_OPT_TYPE_DOUBLE:
			case AV_OPT_TYPE_FLOAT:
			{
				OFX::DoubleParam* curOpt = fetchDoubleParam( name );
				double v = curOpt->getValue();
				_writer.optionSet( type, opt.o, v );
				break;
			}
			case AV_OPT_TYPE_STRING:
			{
				OFX::StringParam* curOpt = fetchStringParam( name );
				std::string v = curOpt->getValue();
				_writer.optionSet( type, opt.o, v );
				break;
			}
			case AV_OPT_TYPE_RATIONAL:
			{
				OFX::Int2DParam* curOpt = fetchInt2DParam( name );
				int vn = curOpt->getValue().x;
				int vd = curOpt->getValue().y;
				_writer.optionSet( type, opt.o, vn, vd );
				break;
			}
			case AV_OPT_TYPE_PIXEL_FMT:
			{
				TUTTLE_COUT( "unsupported pixel fmt parameter" );
				break;
			}
#if LIBAVUTIL_VERSION_INT >= AV_VERSION_INT( 52, 3, 100 )
                        case AV_OPT_TYPE_SAMPLE_FMT:
                        {
                                TUTTLE_COUT( "unsupported sample format parameter" );
                                break;
                        }
#endif
			case AV_OPT_TYPE_IMAGE_SIZE:
			{
				OFX::Int2DParam* curOpt = fetchInt2DParam( name );
				int vn = curOpt->getValue().x;
				int vd = curOpt->getValue().y;
				_writer.optionSetImageSize( type, opt.o, vn, vd );
				break;
			}
			case AV_OPT_TYPE_BINARY:
			{
				OFX::StringParam* curOpt = fetchStringParam( name );
				std::string v = curOpt->getValue();
				_writer.optionSet( type, opt.o, v );
				break;
			}
			case AV_OPT_TYPE_CONST:
			{
				break;
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
						bool v = curOpt->getValue();
						_writer.optionSet( type, opt.o, v, opt.class_name );
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

int convertIntWithOptionalUnit( const std::string& param, const std::string& stringValue )
{
	int intValue = 0;
	if( std::isdigit( stringValue[ stringValue.length()-1 ] ) )
	{
		try
		{
			intValue = boost::lexical_cast<int>( stringValue );
		}
		catch( ... )
		{
			TUTTLE_CERR( "FFMpeg writer: parameter " << param << " can't convert the value: " << stringValue );
		}
	}
	else
	{
		char unit = stringValue[ stringValue.length()-1 ];
		std::string val = stringValue;
		val.erase( val.length()-1 ,1 );
		try
		{
			intValue = boost::lexical_cast<int>( val );
		}
		catch( ... )
		{
			TUTTLE_CERR( "FFMpeg writer: parameter " << param << " can't convert the value: " << stringValue );
		}
		
		switch( unit )
		{
			case 'k':
				intValue *= 1024;
				break;
			case 'M':
				intValue *= 1024 * 1024;
				break;
			case 'G':
				intValue *= 1024 * 1024 * 1024;
				break;
			default:
				TUTTLE_CERR("FFMpeg writer: unknown unit " << unit << " for parameter " << param );
				break;
		}
	}
	return intValue;
}

void FFMpegWriterPlugin::setParameters( const PresetParameters& parameters )
{
	BOOST_FOREACH( PresetParameters::value_type param, parameters )
	{
		//TUTTLE_COUT_VAR2( param.first, param.second );
			if( paramExists( param.first ) )
			{
				std::string value = param.second.at( 0 );
				switch( getParamType( param.first ) )
				{
					case OFX::eStringParam:
					{
						OFX::StringParam* fetchParam = fetchStringParam( param.first );
						fetchParam->setValue( value );
						break;
					}
					case OFX::eIntParam:
					{
						OFX::IntParam* fetchParam = fetchIntParam( param.first );
						fetchParam->setValue( convertIntWithOptionalUnit( param.first, value ) );
						break;
					}
					case OFX::eInt2DParam:
					{
						OFX::Int2DParam* fetchParam = fetchInt2DParam( param.first );
						if( param.second.size() == 2 )
						{
							int x = convertIntWithOptionalUnit( param.first, param.second.at( 0 ) );
							int y = convertIntWithOptionalUnit( param.first, param.second.at( 1 ) );
							fetchParam->setValue( x, y );
						}
						else
						{
							TUTTLE_CERR("FFMpeg writer: unable to set an 2D Int value with " << param.second.size() << " parameters." );
						}
						break;
					}
					case OFX::eInt3DParam:
					{
						OFX::Int3DParam* fetchParam = fetchInt3DParam( param.first );
						if( param.second.size() == 3 )
						{
							int x = convertIntWithOptionalUnit( param.first, param.second.at( 0 ) );
							int y = convertIntWithOptionalUnit( param.first, param.second.at( 1 ) );
							int z = convertIntWithOptionalUnit( param.first, param.second.at( 2 ) );
							fetchParam->setValue( x, y, z );
						}
						else
						{
							TUTTLE_CERR("FFMpeg writer: unable to set an 3D Int value with " << param.second.size() << " parameters." );
						}
						break;
					}
					case OFX::eDoubleParam:
					{
						OFX::DoubleParam* fetchParam = fetchDoubleParam( param.first );
						try
						{
							double value = boost::lexical_cast<double>( value );
							fetchParam->setValue( value );
						}
						catch( ... )
						{
							TUTTLE_CERR( "FFMpeg writer: parameter " << param.first << " can't convert the value: " << value );
						}
						break;
					}
					case OFX::eDouble2DParam:
					{
						OFX::Double2DParam* fetchParam = fetchDouble2DParam( param.first );
						if( param.second.size() == 2 )
						{
							try
							{
								double x = boost::lexical_cast<double>( param.second.at( 0 ) );
								double y = boost::lexical_cast<double>( param.second.at( 1 ) );
								fetchParam->setValue( x, y );
							}
							catch( ... )
							{
								TUTTLE_CERR( "FFMpeg writer: parameter " << param.first << " can't convert the value: [" << param.second.at( 0 ) << ", " << param.second.at( 1 ) << "]" );
							}
						}
						else
						{
							TUTTLE_CERR("FFMpeg writer: unable to set an 2D Double value with " << param.second.size() << " parameters." );
						}
						break;
					}
					case OFX::eDouble3DParam:
					{
						OFX::Double3DParam* fetchParam = fetchDouble3DParam( param.first );
						if( param.second.size() == 3 )
						{
							try
							{
								double x = boost::lexical_cast<double>( param.second.at( 0 ) );
								double y = boost::lexical_cast<double>( param.second.at( 1 ) );
								double z = boost::lexical_cast<double>( param.second.at( 2 ) );
								fetchParam->setValue( x, y, z );
							}
							catch( ... )
							{
								TUTTLE_CERR( "FFMpeg writer: parameter " << param.first << " can't convert the value: [" << param.second.at( 0 ) << ", " << param.second.at( 1 ) << ", " << param.second.at( 2 ) << "]" );
							}
						}
						else
						{
							TUTTLE_CERR("FFMpeg writer: unable to set an 3D Double value with " << param.second.size() << " parameters." );
						}
						break;
					}
					case OFX::eRGBParam:
					{
						OFX::RGBParam* fetchParam = fetchRGBParam( param.first );
						if( param.second.size() == 3 )
						{
							try
							{
								double r = boost::lexical_cast<double>( param.second.at( 0 ) );
								double g = boost::lexical_cast<double>( param.second.at( 1 ) );
								double b = boost::lexical_cast<double>( param.second.at( 2 ) );
								fetchParam->setValue( r, g, b );
							}
							catch( ... )
							{
								TUTTLE_CERR( "FFMpeg writer: parameter " << param.first << " can't convert the value: [" << param.second.at( 0 ) << ", " << param.second.at( 1 ) << ", " << param.second.at( 2 ) << "]" );
							}
						}
						else
						{
							TUTTLE_CERR("FFMpeg writer: unable to set an RGB value with " << param.second.size() << " parameters." );
						}
						break;
					}
					case OFX::eRGBAParam:
					{
						OFX::RGBAParam* fetchParam = fetchRGBAParam( param.first );
						if( param.second.size() == 4 )
						{
							try
							{
								double r = boost::lexical_cast<double>( param.second.at( 0 ) );
								double g = boost::lexical_cast<double>( param.second.at( 1 ) );
								double b = boost::lexical_cast<double>( param.second.at( 2 ) );
								double a = boost::lexical_cast<double>( param.second.at( 3 ) );
								fetchParam->setValue( r, g, b, a );
							}
							catch( ... )
							{
								TUTTLE_CERR( "FFMpeg writer: parameter " << param.first << " can't convert the value: [" << param.second.at( 0 ) << ", " << param.second.at( 1 ) << ", " << param.second.at( 2 ) << ", " << param.second.at( 3 ) << "]" );
							}
						}
						else
						{
							TUTTLE_CERR("FFMpeg writer: unable to set an RGBA value with " << param.second.size() << " parameters." );
						}
						break;
					}
					case OFX::eBooleanParam:
					{
						OFX::BooleanParam* fetchParam = fetchBooleanParam( param.first );
						bool boolValue = ( value == "1" || boost::iequals(value, "y") || boost::iequals(value, "yes") || boost::iequals(value, "true") );
						fetchParam->setValue( boolValue );
						break;
					}
					case OFX::eChoiceParam:
					{
						OFX::ChoiceParam* fetchParam = fetchChoiceParam( param.first );
						int index = -1;
						for( int i = 0; i< fetchParam->getProps().propGetDimension( "OfxParamPropChoiceOption" ); i++ )
						{
							if( fetchParam->getProps().propGetString( "OfxParamPropChoiceOption", i ) == value )
							{
								index = i;
								break;
							}
						}
						if( index > -1 )
							fetchParam->setValue( index );
						else
							TUTTLE_CERR( "FFMpeg writer: unable to find value " << value << " for parameter " << param.first );
						break;
					}
					case OFX::ePushButtonParam:
					case OFX::eParametricParam:
					case OFX::eCustomParam:
					case OFX::eGroupParam:
					case OFX::ePageParam:
					case OFX::eCameraParam:
					default:
					{
						break;
					}
				}
			}
			else
			{
				TUTTLE_CERR( "FFMpeg writer: parameter " << param.first << " not exist." );
			}
	}
}

void FFMpegWriterPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
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
		//TUTTLE_COUT( "preset change " << _paramMainPreset->getValue() );
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
		for( int id = 0; id < idFormatList.size(); id++ )
		{
			if( idFormatList.at( id ) == p.getFormatID() )
			{
				formatIndex = id + 1;
				break;
			}
		}
		
		for( int id = 0; id < idVideoList.size(); id++ )
		{
			if( idVideoList.at( id ) == p.getVideoID() )
			{
				videoIndex = id + 1;
				break;
			}
		}
		
		
		for( int id = 0; id < idAudioList.size(); id++ )
		{
			if( idAudioList.at( id ) == p.getAudioID() )
			{
				audioIndex = id + 1;
				break;
			}
		}
		/*
		TUTTLE_COUT( "set format at " << formatIndex );
		TUTTLE_COUT( "set video at " << formatIndex );
		TUTTLE_COUT( "set audio at " << formatIndex );*/
		
		_paramFormatPreset->setValue( formatIndex );
		_paramVideoCodecPreset->setValue( videoIndex );
		_paramAudioCodecPreset->setValue( audioIndex );
	}
	
	if( paramName == kParamFormatPreset )
	{
		//TUTTLE_COUT( "preset change " << _paramFormatPreset->getValue() );
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
	_initWriter = false;
	WriterPlugin::beginSequenceRender( args );
	
	FFMpegProcessParams params = getProcessParams();
	
	_writer.setFilename    ( params._filepath );
	_writer.setFormat      ( params._format );
	_writer.setVideoCodec  ( params._videoCodec );
	_writer.setFps         ( _clipSrc->getFrameRate() );
	_writer.setAspectRatio ( _clipSrc->getPixelAspectRatio() );
	_writer.setPixelFormat ( params._videoPixelFormat );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FFMpegWriterPlugin::render( const OFX::RenderArguments& args )
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
		setParameters( eAVParamFormat, (void*)avFormatContext, AV_OPT_FLAG_ENCODING_PARAM, 0 );
		
		std::string formatName = _writer.getFormatsShort( ).at(_paramFormat->getValue() );
		setParameters( eAVParamFormat, _writer.getFormatPrivOpts(), formatName );
		
		// set Video Codec parameters
		AVCodecContext* avCodecContext;
	#if LIBAVCODEC_VERSION_INT > AV_VERSION_INT( 53, 8, 0 )
		avCodecContext = avcodec_alloc_context();
		// deprecated in the same version
		//avCodecContext = avcodec_alloc_context2( AVMEDIA_TYPE_UNKNOWN );
	#else
		AVCodec* avCodec = NULL;
		avCodecContext = avcodec_alloc_context3( avCodec );
	#endif
		setParameters( eAVParamVideo, (void*)avCodecContext, AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM, 0 );
		
		std::string codecName = _writer.getVideoCodecsShort( ).at(_paramVideoCodec->getValue() );
		setParameters( eAVParamVideo, _writer.getVideoCodecPrivOpts(), codecName );
		
		// set Audio Codec parameters
		//codecName = _writer.getAudioCodecsShort( ).at(_paramAudioCodec->getValue() );
		//setParameters( eAVParamAudio, _writer.getAudioCodecPrivOpts(), codecName );
		
		_initWriter = true;
	}
	
	doGilRender<FFMpegWriterProcess>( *this, args );
}

void FFMpegWriterPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{
	_writer.finish();
	_initWriter = false;
}

}
}
}
}
