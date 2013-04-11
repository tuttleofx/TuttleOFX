#include "LibAVOptions.hpp"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <map>

namespace tuttle {
namespace plugin {
namespace av {

AVOptionPlugin::AVOptionPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	
}

int AVOptionPlugin::convertIntWithOptionalUnit( const std::string& param, const std::string& stringValue )
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
			TUTTLE_CERR( "avwriter: parameter " << param << " can't convert the value: " << stringValue );
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
			TUTTLE_CERR( "avwriter: parameter " << param << " can't convert the value: " << stringValue );
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
				TUTTLE_CERR("avwriter: unknown unit " << unit << " for parameter " << param );
				break;
		}
	}
	return intValue;
}

void AVOptionPlugin::setParameters( LibAVVideoWriter& writer, const EAVParamType& type, void* av_class, int req_flags, int rej_flags )
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
			OFX::GroupParam* curOpt = fetchGroupParam( name );
			groups.push_back( curOpt );
			continue;
		}
		if( opt->unit && opt->type == AV_OPT_TYPE_INT )
		{
			OFX::ChoiceParam* curOpt = fetchChoiceParam( opt->name );
			//TUTTLE_COUT_VAR3( opt->name, opt->unit, curOpt->getValue() );
			
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
				OFX::BooleanParam* curOpt = fetchBooleanParam( opt->name );
				bool v = curOpt->getValue();
				writer.optionSet( type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_INT:
			case AV_OPT_TYPE_INT64:
			{
				OFX::IntParam* curOpt = fetchIntParam( opt->name );
				int v = curOpt->getValue();
				writer.optionSet( type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_DOUBLE:
			case AV_OPT_TYPE_FLOAT:
			{
				OFX::DoubleParam* curOpt = fetchDoubleParam( opt->name );
				double v = curOpt->getValue();
				writer.optionSet( type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_STRING:
			{
				OFX::StringParam* curOpt = fetchStringParam( opt->name );
				std::string v = curOpt->getValue();
				writer.optionSet( type, *opt, v );
				break;
			}
			case AV_OPT_TYPE_RATIONAL:
			{
				OFX::Int2DParam* curOpt = fetchInt2DParam( opt->name );
				int vn = curOpt->getValue().x;
				int vd = curOpt->getValue().y;
				writer.optionSet( type, *opt, vn, vd );
				break;
			}
			case AV_OPT_TYPE_BINARY:
			{
				OFX::StringParam* curOpt = fetchStringParam( opt->name );
				std::string v = curOpt->getValue();
				writer.optionSet( type, *opt, v );
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
				for( size_t i = 0; i < unit.size(); i++ )
				{
					if( opt->unit == unit.at( i ) )
					{
						//TUTTLE_COUT( "options " << opt->name << " to " << choices.at(i)->getName() );
						
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
							//TUTTLE_COUT( "options "  << choices.at(i)->getName() << " = " << choiceValue.at(i) << " - " << opt->name << " (" << value << ")" );
							writer.optionSet( type, *choiceOpt.at(i), value );
						}
					}
				}
				BOOST_FOREACH( OFX::GroupParam* g, groups )
				{
					std::string name = "g_";
					name += opt->unit;
					if( name == g->getName() )
					{
						OFX::BooleanParam* curOpt = fetchBooleanParam( opt->name );
						bool v = curOpt->getValue();
						std::string optName = opt->unit;
						writer.optionSet( type, *opt, v, optName );
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

void AVOptionPlugin::setParameters( LibAVVideoWriter& writer, const EAVParamType& type, const std::vector<AVPrivOption>& avPrivOpts, const std::string& codec )
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
			writer.optionSet( type, opt.o, v );
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
				writer.optionSet( type, opt.o, v );
				break;
			}
			case AV_OPT_TYPE_INT:
			case AV_OPT_TYPE_INT64:
			{
				OFX::IntParam* curOpt = fetchIntParam( name );
				int v = curOpt->getValue();
				writer.optionSet( type, opt.o, v );
				break;
			}
			case AV_OPT_TYPE_DOUBLE:
			case AV_OPT_TYPE_FLOAT:
			{
				OFX::DoubleParam* curOpt = fetchDoubleParam( name );
				double v = curOpt->getValue();
				writer.optionSet( type, opt.o, v );
				break;
			}
			case AV_OPT_TYPE_STRING:
			{
				OFX::StringParam* curOpt = fetchStringParam( name );
				std::string v = curOpt->getValue();
				writer.optionSet( type, opt.o, v );
				break;
			}
			case AV_OPT_TYPE_RATIONAL:
			{
				OFX::Int2DParam* curOpt = fetchInt2DParam( name );
				int vn = curOpt->getValue().x;
				int vd = curOpt->getValue().y;
				writer.optionSet( type, opt.o, vn, vd );
				break;
			}
			case AV_OPT_TYPE_BINARY:
			{
				OFX::StringParam* curOpt = fetchStringParam( name );
				std::string v = curOpt->getValue();
				writer.optionSet( type, opt.o, v );
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
						writer.optionSet( type, opt.o, v, opt.class_name );
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

void AVOptionPlugin::setParameters( const PresetParameters& parameters )
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
							TUTTLE_CERR("avwriter: unable to set an 2D Int value with " << param.second.size() << " parameters." );
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
							TUTTLE_CERR("avwriter: unable to set an 3D Int value with " << param.second.size() << " parameters." );
						}
						break;
					}
					case OFX::eDoubleParam:
					{
						OFX::DoubleParam* fetchParam = fetchDoubleParam( param.first );
						try
						{
							double value = 0.0;
							value = boost::lexical_cast<double>( value );
							fetchParam->setValue( value );
						}
						catch( ... )
						{
							TUTTLE_CERR( "avwriter: parameter " << param.first << " can't convert the value: " << value );
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
								TUTTLE_CERR( "avwriter: parameter " << param.first << " can't convert the value: [" << param.second.at( 0 ) << ", " << param.second.at( 1 ) << "]" );
							}
						}
						else
						{
							TUTTLE_CERR("avwriter: unable to set an 2D Double value with " << param.second.size() << " parameters." );
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
								TUTTLE_CERR( "avwriter: parameter " << param.first << " can't convert the value: [" << param.second.at( 0 ) << ", " << param.second.at( 1 ) << ", " << param.second.at( 2 ) << "]" );
							}
						}
						else
						{
							TUTTLE_CERR("avwriter: unable to set an 3D Double value with " << param.second.size() << " parameters." );
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
								TUTTLE_CERR( "avwriter: parameter " << param.first << " can't convert the value: [" << param.second.at( 0 ) << ", " << param.second.at( 1 ) << ", " << param.second.at( 2 ) << "]" );
							}
						}
						else
						{
							TUTTLE_CERR("avwriter: unable to set an RGB value with " << param.second.size() << " parameters." );
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
								TUTTLE_CERR( "avwriter: parameter " << param.first << " can't convert the value: [" << param.second.at( 0 ) << ", " << param.second.at( 1 ) << ", " << param.second.at( 2 ) << ", " << param.second.at( 3 ) << "]" );
							}
						}
						else
						{
							TUTTLE_CERR("avwriter: unable to set an RGBA value with " << param.second.size() << " parameters." );
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
							TUTTLE_CERR( "avwriter: unable to find value " << value << " for parameter " << param.first );
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
				TUTTLE_CERR( "avwriter: parameter " << param.first << " not exist." );
			}
	}
}


}
}
}
