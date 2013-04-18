#include "LibAVOptionsFactory.hpp"

#include <boost/foreach.hpp>

namespace tuttle {
namespace plugin {
namespace av {

void addOptionsFromAVOption( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, void* av_class, int req_flags, int rej_flags )
{
	std::vector<OFX::ChoiceParamDescriptor*> choices;
	std::vector<std::string> unit;
	std::vector<int> defaultEnumChoices;
	std::vector<int> defaultChoices;
	std::vector<OFX::GroupParamDescriptor*> groups;
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
			OFX::GroupParamDescriptor* param = desc.defineGroupParam( name );
			if( opt->help )
				param->setHint( opt->help );
			param->setParent( group );
			//param->setAsTab( );
			groups.push_back( param );
			continue;
		}
		if( opt->unit && opt->type == AV_OPT_TYPE_INT )
		{
			OFX::ChoiceParamDescriptor* param = desc.defineChoiceParam( opt->name );
			if( opt->help )
				param->setHint( opt->help );
			param->setParent( group );
			choices.push_back( param );
			unit.push_back( opt->unit );
			defaultEnumChoices.push_back( opt->default_val.dbl );
			//TUTTLE_COUT( opt->unit << " = " << opt->default_val.dbl );
			continue;
		}
		
		switch( opt->type )
		{
			case AV_OPT_TYPE_FLAGS:
			{
				OFX::BooleanParamDescriptor* param = desc.defineBooleanParam( opt->name );
				param->setDefault( opt->default_val.i64 );
				if( opt->help )
					param->setHint( opt->help );
				param->setParent( group );
				break;
			}
			case AV_OPT_TYPE_INT:
			case AV_OPT_TYPE_INT64:
			{
				OFX::IntParamDescriptor* param = desc.defineIntParam( opt->name );
				param->setDefault( opt->default_val.i64 );
				param->setRange( opt->min, opt->max );
				param->setDisplayRange( opt->min, opt->max );
				if( opt->help )
					param->setHint( opt->help );
				param->setParent( group );
				break;
			}
			case AV_OPT_TYPE_DOUBLE:
			case AV_OPT_TYPE_FLOAT:
			{
				OFX::DoubleParamDescriptor* param = desc.defineDoubleParam( opt->name );
				param->setDefault( opt->default_val.dbl );
				param->setRange( opt->min, opt->max );
				param->setDisplayRange( opt->min, opt->max );
				if( opt->help )
					param->setHint( opt->help );
				param->setParent( group );
				break;
			}
			case AV_OPT_TYPE_STRING:
			{
				OFX::StringParamDescriptor* param = desc.defineStringParam( opt->name );
				if( opt->default_val.str )
					param->setDefault( opt->default_val.str );
				if( opt->help )
					param->setHint( opt->help );
				param->setParent( group );
				break;
			}
			case AV_OPT_TYPE_RATIONAL:
			{
				OFX::Int2DParamDescriptor* param = desc.defineInt2DParam( opt->name );
				param->setDefault( opt->default_val.q.num, opt->default_val.q.den );
				if( opt->help )
					param->setHint( opt->help );
				param->setParent( group );
				break;
			}
			case AV_OPT_TYPE_BINARY:
			{
				OFX::StringParamDescriptor* param = desc.defineStringParam( opt->name );
				if( opt->help )
					param->setHint( opt->help );
				param->setParent( group );
				break;
			}
			case AV_OPT_TYPE_CONST:
			{
				break;
			}
			default:
			{
				TUTTLE_COUT( "AudioVideo: undefined type for " << opt->name );
			}
		}
	}

	defaultChoices.resize( defaultEnumChoices.size() );
	
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
						//TUTTLE_COUT( "add " << opt->name << " to " << choices.at(i)->getName() );
						if( opt->help )
							choices.at(i)->appendOption( opt->name, opt->help );
						else
							choices.at(i)->appendOption( opt->name );
						if( opt->default_val.dbl == defaultEnumChoices.at(i) )
						{
							defaultChoices.at(i) = choices.at(i)->getNOptions() - 1;
						}
						
					}
				}
				
				BOOST_FOREACH( OFX::GroupParamDescriptor* g, groups )
				{
					std::string name = "g_";
					name += opt->unit;
					if( name == g->getName() )
					{
						OFX::BooleanParamDescriptor* param = desc.defineBooleanParam( opt->name );
						param->setDefault( opt->offset );
						if( opt->help )
							param->setHint( opt->help );
						param->setParent( g );
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
	
	// set default value for enums
	opt = NULL;
	while( (opt = av_opt_next( av_class, opt)  ) != NULL )
	{
		if( !opt ||
			!opt->name ||
			( opt->flags & req_flags ) != req_flags ||
			( opt->flags & rej_flags ) ||
			( opt->unit && opt->type == AV_OPT_TYPE_FLAGS ) )
		{
			continue;
		}
		
		if( opt->unit && opt->type == AV_OPT_TYPE_INT )
		{
			for( size_t i = 0; i < defaultChoices.size(); i++ )
			{
				choices.at(i)->setDefault( defaultChoices.at(i) );
			}
			
		}
	}
}

void addOptionsFromAVOption( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, const std::vector<AVPrivOption>& options )
{
	std::vector<OFX::ChoiceParamDescriptor*> choices;
	std::vector<std::string> unit;
	std::vector<int> defaultEnumChoices;
	std::vector<int> defaultChoices;
	std::vector<OFX::GroupParamDescriptor*> groups;

	BOOST_FOREACH( AVPrivOption opt, options )
	{
		if( opt.o.unit && opt.o.type == AV_OPT_TYPE_FLAGS )
		{
			std::string name = "g_";
			name += opt.class_name;
			name += "_";
			name += opt.o.unit;
			//TUTTLE_COUT( "add " << name );
			OFX::GroupParamDescriptor* param = desc.defineGroupParam( name );
			param->setLabel( opt.o.name );
			if( opt.o.help )
				param->setHint( opt.o.help );
			param->setParent( group );
			//param->setAsTab( );
			groups.push_back( param );
			continue;
		}
		if( opt.o.unit && opt.o.type == AV_OPT_TYPE_INT )
		{
			std::string name = opt.class_name;
			name += "_";
			name += opt.o.name;
			//TUTTLE_COUT( "add " << name );
			OFX::ChoiceParamDescriptor* param = desc.defineChoiceParam( name );
			param->setLabel( opt.o.name );
			if( opt.o.help )
				param->setHint( opt.o.help );
			param->setParent( group );
			choices.push_back( param );
			std::string u = opt.class_name;
			u += "_";
			u += opt.o.unit;
			unit.push_back( u );
			defaultEnumChoices.push_back( opt.o.default_val.dbl );
			//TUTTLE_COUT( opt.o.unit << " = " << opt.o.default_val.dbl );
			continue;
		}
		
		std::string name = opt.class_name;
		name += "_";
		name += opt.o.name;
		//TUTTLE_COUT( "+add " << name );
		switch( opt.o.type )
		{
			case AV_OPT_TYPE_FLAGS:
			{
				OFX::BooleanParamDescriptor* param = desc.defineBooleanParam( name );
				param->setLabel( opt.o.name );
				param->setDefault( opt.o.default_val.i64 );
				if( opt.o.help )
					param->setHint( opt.o.help );
				param->setParent( group );
				break;
			}
			case AV_OPT_TYPE_INT:
			case AV_OPT_TYPE_INT64:
			{
				OFX::IntParamDescriptor* param = desc.defineIntParam( name );
				param->setLabel( opt.o.name );
				param->setDefault( opt.o.default_val.i64 );
				param->setRange( opt.o.min, opt.o.max );
				param->setDisplayRange( opt.o.min, opt.o.max );
				if( opt.o.help )
					param->setHint( opt.o.help );
				param->setParent( group );
				break;
			}
			case AV_OPT_TYPE_DOUBLE:
			case AV_OPT_TYPE_FLOAT:
			{
				OFX::DoubleParamDescriptor* param = desc.defineDoubleParam( name );
				param->setLabel( opt.o.name );
				param->setDefault( opt.o.default_val.dbl );
				param->setRange( opt.o.min, opt.o.max );
				param->setDisplayRange( opt.o.min, opt.o.max );
				if( opt.o.help )
					param->setHint( opt.o.help );
				param->setParent( group );
				break;
			}
			case AV_OPT_TYPE_STRING:
			{
				OFX::StringParamDescriptor* param = desc.defineStringParam( name );
				param->setLabel( opt.o.name );
				if( opt.o.default_val.str )
					param->setDefault( opt.o.default_val.str );
				if( opt.o.help )
					param->setHint( opt.o.help );
				param->setParent( group );
				break;
			}
			case AV_OPT_TYPE_RATIONAL:
			{
				OFX::Int2DParamDescriptor* param = desc.defineInt2DParam( name );
				param->setLabel( opt.o.name );
				param->setDefault( opt.o.default_val.q.num, opt.o.default_val.q.den );
				if( opt.o.help )
					param->setHint( opt.o.help );
				param->setParent( group );
				break;
			}
			case AV_OPT_TYPE_BINARY:
			{
				OFX::StringParamDescriptor* param = desc.defineStringParam( name );
				param->setLabel( opt.o.name );
				if( opt.o.help )
					param->setHint( opt.o.help );
				param->setParent( group );
				break;
			}
			case AV_OPT_TYPE_CONST:
			{
				break;
			}
			default:
			{
				TUTTLE_COUT( "AudioVideo: undefined type for " << opt.o.name );
				
			}
		}
	}

	defaultChoices.resize( defaultEnumChoices.size() );
	
	// adding enum values and flags in groups
	BOOST_FOREACH( AVPrivOption opt, options )
	{
		switch( opt.o.type )
		{
			case AV_OPT_TYPE_CONST:
			{
				for( size_t i = 0; i < unit.size(); i++ )
				{
					std::string u = opt.class_name;
					u += "_";
					u += opt.o.unit;
					if( u == unit.at( i ) )
					{
						//TUTTLE_COUT( "add " << opt.o.name << " to " << choices.at(i)->getName() );
						if( opt.o.help )
							choices.at(i)->appendOption( opt.o.name, opt.o.help );
						else
							choices.at(i)->appendOption( opt.o.name );
						if( opt.o.default_val.dbl == defaultEnumChoices.at(i) )
						{
							defaultChoices.at(i) = choices.at(i)->getNOptions() - 1;
						}
						
					}
				}
				BOOST_FOREACH( OFX::GroupParamDescriptor* g, groups )
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
						OFX::BooleanParamDescriptor* param = desc.defineBooleanParam( name );
						param->setLabel( opt.o.name );
						param->setDefault( opt.o.offset );
						if( opt.o.help )
							param->setHint( opt.o.help );
						param->setParent( g );
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
	
	// set default value for enums
	BOOST_FOREACH( AVPrivOption opt, options )
	{
		if( opt.o.unit && opt.o.type == AV_OPT_TYPE_INT )
		{
			for( size_t i = 0; i < defaultChoices.size(); i++ )
			{
				choices.at(i)->setDefault( defaultChoices.at(i) );
			}
			
		}
	}
}

}
}
}
