#include "util.hpp"

#include <AvTranscoder/Option.hpp>

#include <boost/foreach.hpp>

#include <iostream>

namespace tuttle {
namespace plugin {
namespace av {
namespace common {

void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, avtranscoder::OptionLoader::OptionArray& optionsArray, const std::string& prefix )
{
	OFX::ParamDescriptor* param = NULL;
	BOOST_FOREACH( avtranscoder::Option& option, optionsArray )
	{
		std::string name = prefix;
		name += option.getName();
		
		switch( option.getType() )
		{
			case avtranscoder::TypeBool:
			{
				OFX::BooleanParamDescriptor* boolParam = desc.defineBooleanParam( name );
				boolParam->setDefault( option.getDefaultValueBool() );
				param = boolParam;
				break;
			}
			case avtranscoder::TypeInt:
			{
				OFX::IntParamDescriptor* intParam = desc.defineIntParam( option.getName() );
				intParam->setDefault( option.getDefaultValueInt() );
				intParam->setRange( option.getMin(), option.getMax() );
				intParam->setDisplayRange( option.getMin(), option.getMax() );
				param = intParam;
				break;
			}
			case avtranscoder::TypeDouble:
			{
				OFX::DoubleParamDescriptor* doubleParam = desc.defineDoubleParam( name );
				doubleParam->setDefault( option.getDefaultValueDouble() );
				doubleParam->setRange( option.getMin(), option.getMax() );
				doubleParam->setDisplayRange( option.getMin(), option.getMax() );
				param = doubleParam;
				break;
			}
			case avtranscoder::TypeString:
			{
				OFX::StringParamDescriptor* strParam = desc.defineStringParam( name );
				strParam->setDefault( option.getDefaultValueString() );
				param = strParam;
				break;
			}
			case avtranscoder::TypeRatio:
			{
				OFX::Int2DParamDescriptor* ratioParam = desc.defineInt2DParam( name );
				ratioParam->setDefault( option.getDefaultValueRatio().first, option.getDefaultValueRatio().second );
				param = ratioParam;
				break;
			}
			case avtranscoder::TypeChoice:
			{
				OFX::ChoiceParamDescriptor* choiceParam = desc.defineChoiceParam( name );
				choiceParam->setDefault( option.getDefaultChildIndex() );
				BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
				{
					choiceParam->appendOption( child.getName(), child.getHelp() );
				}
				param = choiceParam;
				break;
			}
			case avtranscoder::TypeGroup:
			{
				std::string groupName = prefix;
				groupName += "g_";
				groupName += option.getName();
				OFX::GroupParamDescriptor* groupParam = desc.defineGroupParam( groupName );
				BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
				{
					OFX::BooleanParamDescriptor* param = desc.defineBooleanParam( child.getName() );
					param->setLabel( child.getName() );
					param->setDefault( child.getOffset() );
					param->setHint( child.getHelp() );
					param->setParent( groupParam );
				}
				param = groupParam;
				break;
			}
			default:
				break;
		}
		if( param )
		{
			param->setLabel( option.getName() );
			param->setHint( option.getHelp() );
			param->setParent( group );
		}
	}
}

void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group,  avtranscoder::OptionLoader::OptionMap& optionsMap )
{
	// iterate on map keys
	BOOST_FOREACH( avtranscoder::OptionLoader::OptionMap::value_type& subGroupOption, optionsMap )
	{
		OFX::ParamDescriptor* param = NULL;
		
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
					OFX::BooleanParamDescriptor* boolParam = desc.defineBooleanParam( name );
					boolParam->setDefault( option.getDefaultValueBool() );
					param = boolParam;
					break;
				}
				case avtranscoder::TypeInt:
				{
					OFX::IntParamDescriptor* intParam = desc.defineIntParam( name );
					intParam->setDefault( option.getDefaultValueInt() );
					intParam->setRange( option.getMin(), option.getMax() );
					intParam->setDisplayRange( option.getMin(), option.getMax() );
					param = intParam;
					break;
				}
				case avtranscoder::TypeDouble:
				{
					OFX::DoubleParamDescriptor* doubleParam = desc.defineDoubleParam( name );
					doubleParam->setDefault( option.getDefaultValueDouble() );
					doubleParam->setRange( option.getMin(), option.getMax() );
					doubleParam->setDisplayRange( option.getMin(), option.getMax() );
					param = doubleParam;
					break;
				}
				case avtranscoder::TypeString:
				{
					OFX::StringParamDescriptor* strParam = desc.defineStringParam( name );
					strParam->setDefault( option.getDefaultValueString() );
					param = strParam;
					break;
				}
				case avtranscoder::TypeRatio:
				{
					OFX::Int2DParamDescriptor* ratioParam = desc.defineInt2DParam( name );
					ratioParam->setDefault( option.getDefaultValueRatio().first, option.getDefaultValueRatio().second );
					param = ratioParam;
					break;
				}
				case avtranscoder::TypeChoice:
				{
					OFX::ChoiceParamDescriptor* choiceParam = desc.defineChoiceParam( name );
					choiceParam->setDefault( option.getDefaultChildIndex() );
					BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
					{
						choiceParam->appendOption( child.getName(), child.getHelp() );
					}
					param = choiceParam;
					break;
				}
				case avtranscoder::TypeGroup:
				{
					std::string groupName = "g_";
					groupName += subGroupName;
					groupName += "_";
					groupName += option.getName();
					
					OFX::GroupParamDescriptor* groupParam = desc.defineGroupParam( groupName );
					BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
					{
						std::string ChildName = "flags_";
						ChildName += subGroupName;
						ChildName += "_";
						ChildName += child.getName();
						
						OFX::BooleanParamDescriptor* param = desc.defineBooleanParam( ChildName );
						param->setLabel( child.getName() );
						param->setDefault( child.getOffset() );
						param->setHint( child.getHelp() );
						param->setParent( groupParam );
					}
					param = groupParam;
					break;
				}
				default:
					break;
			}
			if( param )
			{
				param->setLabel( option.getName() );
				param->setHint( option.getHelp() );
				param->setParent( group );
			}
		}
	}
}

}
}
}
}
