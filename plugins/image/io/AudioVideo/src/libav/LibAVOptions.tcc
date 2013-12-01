#include "LibAV.hpp"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <map>

namespace tuttle {
namespace plugin {
namespace av {

template< typename IOPlugin >
AVOptionPlugin<IOPlugin>::AVOptionPlugin( OfxImageEffectHandle handle )
	: IOPlugin( handle )
{
	
}

template< typename IOPlugin >
int AVOptionPlugin<IOPlugin>::convertIntWithOptionalUnit( const std::string& param, const std::string& stringValue )
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
			TUTTLE_LOG_ERROR( "avwriter: parameter " << param << " can't convert the value: " << stringValue );
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
			TUTTLE_LOG_ERROR( "avwriter: parameter " << param << " can't convert the value: " << stringValue );
		}
		
		switch( unit )
		{
			case 'k':
				intValue *= 1000;
				break;
			case 'M':
				intValue *= 1000 * 1000;
				break;
			case 'G':
				intValue *= 1000 * 1000 * 1000;
				break;
			default:
				TUTTLE_LOG_ERROR("avwriter: unknown unit " << unit << " for parameter " << param );
				break;
		}
	}
	return intValue;
}

template< typename IOPlugin >
template< typename LibAVVideoRW >
void AVOptionPlugin<IOPlugin>::setParameters( LibAVVideoRW& videoRW, const EAVParamType& type, void* av_class, int req_flags, int rej_flags )
{
	std::vector<OFX::ChoiceParam*> choices;
	std::vector<std::string> unit;
	std::vector<size_t> choiceValue;
	std::vector<const AVOption*> choiceOpt;
	std::map<std::string, size_t> choiceListWithCountOption;
	
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
			OFX::GroupParam* curOpt = IOPlugin::fetchGroupParam( name );
			groups.push_back( curOpt );
			continue;
		}
		if( opt->unit && opt->type == AV_OPT_TYPE_INT )
		{
			OFX::ChoiceParam* curOpt = IOPlugin::fetchChoiceParam( opt->name );
			//TUTTLE_LOG_VAR3( TUTTLE_INFO, opt->name, opt->unit, curOpt->getValue() );
			
			choiceOpt.push_back( opt );
			choices.push_back( curOpt );
			unit.push_back( opt->unit );
			choiceValue.push_back( curOpt->getValue() );
			continue;
		}
		
		switch( opt->type )
		{
			case AV_OPT_TYPE_FLAGS:
			{
				OFX::BooleanParam* curOpt = IOPlugin::fetchBooleanParam( opt->name );
				bool v = curOpt->getValue();
				optionSet( videoRW, type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_INT:
			case AV_OPT_TYPE_INT64:
			{
				OFX::IntParam* curOpt = IOPlugin::fetchIntParam( opt->name );
				int v = curOpt->getValue();
				optionSet( videoRW, type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_DOUBLE:
			case AV_OPT_TYPE_FLOAT:
			{
				OFX::DoubleParam* curOpt = IOPlugin::fetchDoubleParam( opt->name );
				double v = curOpt->getValue();
				optionSet( videoRW, type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_STRING:
			{
				OFX::StringParam* curOpt = IOPlugin::fetchStringParam( opt->name );
				std::string v = curOpt->getValue();
				optionSet( videoRW, type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_RATIONAL:
			{
				OFX::Int2DParam* curOpt = IOPlugin::fetchInt2DParam( opt->name );
				int vn = curOpt->getValue().x;
				int vd = curOpt->getValue().y;
				optionSet( videoRW, type, *opt, vn, vd );
				break;
			}
			case AV_OPT_TYPE_BINARY:
			{
				OFX::StringParam* curOpt = IOPlugin::fetchStringParam( opt->name );
				std::string v = curOpt->getValue();
				optionSet( videoRW, type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_CONST:
			{
				break;
			}
			default:
			{
				TUTTLE_LOG_WARNING( "AudioVideo: undefined type for " << opt->name );
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
				for( size_t i = 0; i < unit.size(); i++ )
				{
					if( opt->unit == unit.at( i ) )
					{
						//TUTTLE_LOG_INFO( "options " << opt->name << " to " << choices.at(i)->getName() );
						
						if( choiceListWithCountOption.find( choices.at(i)->getName() ) == choiceListWithCountOption.end() )
						{
							choiceListWithCountOption[ choices.at(i)->getName() ] = 0;
						}
						else
						{
							choiceListWithCountOption[ choices.at(i)->getName() ] += 1;
						}
						
						if( choiceListWithCountOption[ choices.at(i)->getName() ] == choiceValue.at(i) )
						{
							int value = opt->default_val.i64;
							//TUTTLE_LOG_INFO( "options "  << choices.at(i)->getName() << " = " << choiceValue.at(i) << " - " << opt->name << " (" << value << ")" );
							optionSet( videoRW, type, *choiceOpt.at(i), value );
						}
					}
				}
				BOOST_FOREACH( OFX::GroupParam* g, groups )
				{
					std::string name = "g_";
					name += opt->unit;
					if( name == g->getName() )
					{
						OFX::BooleanParam* curOpt = IOPlugin::fetchBooleanParam( opt->name );
						bool v = curOpt->getValue();
						std::string optName = opt->unit;
						optionSet( videoRW, type, *opt, v, optName );
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

template< typename IOPlugin >
template< typename LibAVVideoRW >
void AVOptionPlugin<IOPlugin>::setParameters( LibAVVideoRW& videoRW, const EAVParamType& type, const std::vector<AVPrivOption>& avPrivOpts, const std::string& codec )
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
			
			OFX::GroupParam* curOpt = IOPlugin::fetchGroupParam( name );
			groups.push_back( curOpt );
			continue;
		}
		if( opt.o.unit && opt.o.type == AV_OPT_TYPE_INT )
		{
			std::string name = opt.class_name;
			name += "_";
			name += opt.o.name;
			
			OFX::ChoiceParam* curOpt = IOPlugin::fetchChoiceParam( name );
			int v = curOpt->getValue();
			optionSet( videoRW, type, opt.o, v );
			continue;
		}
		
		std::string name = opt.class_name;
		name += "_";
		name += opt.o.name;
		
		switch( opt.o.type )
		{
			case AV_OPT_TYPE_FLAGS:
			{
				OFX::BooleanParam* curOpt = IOPlugin::fetchBooleanParam( name );
				bool v = curOpt->getValue();
				optionSet( videoRW, type, opt.o, v );
				break;
			}
			case AV_OPT_TYPE_INT:
			case AV_OPT_TYPE_INT64:
			{
				OFX::IntParam* curOpt = IOPlugin::fetchIntParam( name );
				int v = curOpt->getValue();
				optionSet( videoRW, type, opt.o, v );
				break;
			}
			case AV_OPT_TYPE_DOUBLE:
			case AV_OPT_TYPE_FLOAT:
			{
				OFX::DoubleParam* curOpt = IOPlugin::fetchDoubleParam( name );
				double v = curOpt->getValue();
				optionSet( videoRW, type, opt.o, v );
				break;
			}
			case AV_OPT_TYPE_STRING:
			{
				OFX::StringParam* curOpt = IOPlugin::fetchStringParam( name );
				std::string v = curOpt->getValue();
				optionSet( videoRW, type, opt.o, v );
				break;
			}
			case AV_OPT_TYPE_RATIONAL:
			{
				OFX::Int2DParam* curOpt = IOPlugin::fetchInt2DParam( name );
				int vn = curOpt->getValue().x;
				int vd = curOpt->getValue().y;
				optionSet( videoRW, type, opt.o, vn, vd );
				break;
			}
			case AV_OPT_TYPE_BINARY:
			{
				OFX::StringParam* curOpt = IOPlugin::fetchStringParam( name );
				std::string v = curOpt->getValue();
				optionSet( videoRW, type, opt.o, v );
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
						
						OFX::BooleanParam* curOpt = IOPlugin::fetchBooleanParam( name );
						bool v = curOpt->getValue();
						optionSet( videoRW, type, opt.o, v, opt.class_name );
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

template< typename IOPlugin >
void AVOptionPlugin<IOPlugin>::setParameters( const PresetParameters& parameters )
{
	BOOST_FOREACH( PresetParameters::value_type param, parameters )
	{
		//TUTTLE_LOG_VAR2( TUTTLE_INFO, param.first, param.second );
			if( IOPlugin::paramExists( param.first ) )
			{
				std::string value = param.second.at( 0 );
				switch( IOPlugin::getParamType( param.first ) )
				{
					case OFX::eStringParam:
					{
						OFX::StringParam* fetchParam = IOPlugin::fetchStringParam( param.first );
						fetchParam->setValue( value );
						break;
					}
					case OFX::eIntParam:
					{
						OFX::IntParam* fetchParam = IOPlugin::fetchIntParam( param.first );
						fetchParam->setValue( convertIntWithOptionalUnit( param.first, value ) );
						break;
					}
					case OFX::eInt2DParam:
					{
						OFX::Int2DParam* fetchParam = IOPlugin::fetchInt2DParam( param.first );
						if( param.second.size() == 2 )
						{
							int x = convertIntWithOptionalUnit( param.first, param.second.at( 0 ) );
							int y = convertIntWithOptionalUnit( param.first, param.second.at( 1 ) );
							fetchParam->setValue( x, y );
						}
						else
						{
							TUTTLE_LOG_WARNING( "avwriter: unable to set an 2D Int value with " << param.second.size() << " parameters." );
						}
						break;
					}
					case OFX::eInt3DParam:
					{
						OFX::Int3DParam* fetchParam = IOPlugin::fetchInt3DParam( param.first );
						if( param.second.size() == 3 )
						{
							int x = convertIntWithOptionalUnit( param.first, param.second.at( 0 ) );
							int y = convertIntWithOptionalUnit( param.first, param.second.at( 1 ) );
							int z = convertIntWithOptionalUnit( param.first, param.second.at( 2 ) );
							fetchParam->setValue( x, y, z );
						}
						else
						{
							TUTTLE_LOG_WARNING( "avwriter: unable to set an 3D Int value with " << param.second.size() << " parameters." );
						}
						break;
					}
					case OFX::eDoubleParam:
					{
						OFX::DoubleParam* fetchParam = IOPlugin::fetchDoubleParam( param.first );
						try
						{
							double value = 0.0;
							value = boost::lexical_cast<double>( value );
							fetchParam->setValue( value );
						}
						catch( ... )
						{
							TUTTLE_LOG_WARNING( "avwriter: parameter " << param.first << " can't convert the value: " << value );
						}
						break;
					}
					case OFX::eDouble2DParam:
					{
						OFX::Double2DParam* fetchParam = IOPlugin::fetchDouble2DParam( param.first );
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
								TUTTLE_LOG_WARNING( "avwriter: parameter " << param.first << " can't convert the value: [" << param.second.at( 0 ) << ", " << param.second.at( 1 ) << "]" );
							}
						}
						else
						{
							TUTTLE_LOG_WARNING("avwriter: unable to set an 2D Double value with " << param.second.size() << " parameters." );
						}
						break;
					}
					case OFX::eDouble3DParam:
					{
						OFX::Double3DParam* fetchParam = IOPlugin::fetchDouble3DParam( param.first );
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
								TUTTLE_LOG_WARNING( "avwriter: parameter " << param.first << " can't convert the value: [" << param.second.at( 0 ) << ", " << param.second.at( 1 ) << ", " << param.second.at( 2 ) << "]" );
							}
						}
						else
						{
							TUTTLE_LOG_WARNING("avwriter: unable to set an 3D Double value with " << param.second.size() << " parameters." );
						}
						break;
					}
					case OFX::eRGBParam:
					{
						OFX::RGBParam* fetchParam = IOPlugin::fetchRGBParam( param.first );
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
								TUTTLE_LOG_WARNING( "avwriter: parameter " << param.first << " can't convert the value: [" << param.second.at( 0 ) << ", " << param.second.at( 1 ) << ", " << param.second.at( 2 ) << "]" );
							}
						}
						else
						{
							TUTTLE_LOG_WARNING("avwriter: unable to set an RGB value with " << param.second.size() << " parameters." );
						}
						break;
					}
					case OFX::eRGBAParam:
					{
						OFX::RGBAParam* fetchParam = IOPlugin::fetchRGBAParam( param.first );
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
								TUTTLE_LOG_WARNING( "avwriter: parameter " << param.first << " can't convert the value: [" << param.second.at( 0 ) << ", " << param.second.at( 1 ) << ", " << param.second.at( 2 ) << ", " << param.second.at( 3 ) << "]" );
							}
						}
						else
						{
							TUTTLE_LOG_WARNING("avwriter: unable to set an RGBA value with " << param.second.size() << " parameters." );
						}
						break;
					}
					case OFX::eBooleanParam:
					{
						OFX::BooleanParam* fetchParam = IOPlugin::fetchBooleanParam( param.first );
						bool boolValue = ( value == "1" || boost::iequals(value, "y") || boost::iequals(value, "yes") || boost::iequals(value, "true") );
						fetchParam->setValue( boolValue );
						break;
					}
					case OFX::eChoiceParam:
					{
						OFX::ChoiceParam* fetchParam = IOPlugin::fetchChoiceParam( param.first );
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
							TUTTLE_LOG_WARNING( "avwriter: unable to find value " << value << " for parameter " << param.first );
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
				TUTTLE_LOG_WARNING( "avwriter: parameter " << param.first << " not exist." );
			}
	}
}

template< typename IOPlugin >
template< typename LibAVVideoRW >
void AVOptionPlugin<IOPlugin>::optionSet( LibAVVideoRW& videoRW, const EAVParamType& type, const AVOption& opt, bool& value )
{
	int error = 0;
	switch( type )
	{
		case eAVParamFormat:
		{
			error = av_opt_set_int( videoRW._avFormatOptions, opt.name, value, AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamVideo:
		{
			error = av_opt_set_int( videoRW._stream->codec, opt.name, value, AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamAudio:
		{
			error = 0;
			//av_opt_set_int( _avFormatOptions, opt.name, value, 0 );
			break;
		}
	}
	if( error )
		TUTTLE_LOG_WARNING( "avwriter: " << LibAV::libavError_toString( error ) << " : " << opt.name << " ( " << ( value ? "True" : "False" ) << " )" );
}


template< typename IOPlugin >
template< typename LibAVVideoRW >
void AVOptionPlugin<IOPlugin>::optionSet( LibAVVideoRW& videoRW, const EAVParamType& type, const AVOption &opt, bool& value, std::string& valueToSetFlag )
{
	int error = 0;
	int64_t optVal;
	void* obj = NULL;
	switch( type )
	{
		case eAVParamFormat:
		{
			obj = (void*) videoRW._avFormatOptions;
			break;
		}
		case eAVParamVideo:
		{
			obj = (void*) videoRW._stream->codec;
			break;
		}
		case eAVParamAudio:
		{
			//av_opt_set_int( _avFormatOptions, opt.name, value, 0 );
			return;
		}
	}
	
	error = av_opt_get_int( obj, opt.unit, AV_OPT_SEARCH_CHILDREN, &optVal );
	
	if( value )
		optVal = optVal | (int64_t) opt.default_val.i64;
	else
		optVal = optVal &~(int64_t) opt.default_val.i64;
	
	error = av_opt_set_int( obj, opt.unit, optVal, AV_OPT_SEARCH_CHILDREN );
	
	if( error )
		TUTTLE_LOG_WARNING( "avwriter: " << LibAV::libavError_toString( error ) << " : " << valueToSetFlag << " ( " <<  opt.name << " = " << ( value ? "True" : "False" ) << " )" );
}

template< typename IOPlugin >
template< typename LibAVVideoRW >
void AVOptionPlugin<IOPlugin>::optionSet( LibAVVideoRW& videoRW, const EAVParamType& type, const AVOption& opt, int& value )
{
	int error = 0;
	switch( type )
	{
		case eAVParamFormat:
		{
			error = av_opt_set_int( videoRW._avFormatOptions, opt.name, value, AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamVideo:
		{
			
			error = av_opt_set_int( videoRW._stream->codec, opt.name, value, AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamAudio:
		{
			error = 0;
			//av_opt_set_int( _avFormatOptions, opt.name, value, 0 );
			break;
		}
	}
	if( error )
		TUTTLE_LOG_WARNING( "avwriter: " << LibAV::libavError_toString( error ) << " : " << opt.name << " ( " << value << " )" );
}

template< typename IOPlugin >
template< typename LibAVVideoRW >
void AVOptionPlugin<IOPlugin>::optionSet( LibAVVideoRW& videoRW, const EAVParamType& type, const AVOption &opt, double &value )
{
	int error = 0;
	switch( type )
	{
		case eAVParamFormat:
		{
			error = av_opt_set_double( videoRW._avFormatOptions, opt.name, value, AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamVideo:
		{
			error = av_opt_set_double( videoRW._stream->codec, opt.name, value, AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamAudio:
		{
			error = 0;
			//av_opt_set_double( _avFormatOptions, opt.name, value, 0 );
			break;
		}
	}
	if( error )
		TUTTLE_LOG_WARNING( "avwriter: " << LibAV::libavError_toString( error ) << " : " << opt.name << " ( " << value << " )" );
}

template< typename IOPlugin >
template< typename LibAVVideoRW >
void AVOptionPlugin<IOPlugin>::optionSet( LibAVVideoRW& videoRW, const EAVParamType& type, const AVOption &opt, int &valueNum, int& valueDen )
{
	int error = 0;
	AVRational q;
	q.num = valueNum;
	q.den = valueDen;
	switch( type )
	{
		case eAVParamFormat:
		{
			error = av_opt_set_q( videoRW._avFormatOptions, opt.name, q, AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamVideo:
		{
			error = av_opt_set_q( videoRW._stream->codec, opt.name, q, AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamAudio:
		{
			error = 0;
			//av_opt_set_q( _avFormatOptions, opt.name, q, 0 );
			break;
		}
	}
	if( error )
		TUTTLE_LOG_WARNING( "avwriter: " << LibAV::libavError_toString( error ) << " : " << opt.name << " ( " << valueNum << "/" << valueDen<< " )" );
}

template< typename IOPlugin >
template< typename LibAVVideoRW >
void AVOptionPlugin<IOPlugin>::optionSet( LibAVVideoRW& videoRW, const EAVParamType& type, const AVOption &opt, std::string &value )
{
	int error = 0;
	if( ! value.length() )
		return;
	switch( type )
	{
		case eAVParamFormat:
		{
			error = av_opt_set( videoRW._avFormatOptions, opt.name, value.c_str(), AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamVideo:
		{
			error = av_opt_set( videoRW._stream->codec, opt.name, value.c_str(), AV_OPT_SEARCH_CHILDREN );
			break;
		}
		case eAVParamAudio:
		{
			error = 0;
			//av_opt_set( _avFormatOptions, opt.name, value.c_str(), 0 );
			break;
		}
	}
	if( error )
		TUTTLE_LOG_WARNING( "avwriter: " << LibAV::libavError_toString( error ) << " : " << opt.name << " ( " << value << " )" );
}


}
}
}
