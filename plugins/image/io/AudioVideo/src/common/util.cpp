#include "util.hpp"

#include <writer/AVWriterDefinitions.hpp>

#include <AvTranscoder/Option.hpp>

#include <boost/foreach.hpp>

#include <iostream>

namespace tuttle {
namespace plugin {
namespace av {
namespace common {

CustomParams::OptionsForPreset CustomParams::getOptionsNameAndValue( const std::string& subGroupName )
{
	OptionsForPreset optionsNameAndValue;

	BOOST_FOREACH( OFX::BooleanParam* param, _paramBoolean )
	{
		if( ! subGroupName.empty() && param->getName().find( "_" + subGroupName + "_" ) == std::string::npos )
			continue;

		// FFMPEG exception with the flags
		if( param->getName().find( "flags_" ) != std::string::npos )
		{
			std::string optionValue;
			if( param->getValue() )
				optionValue.append( "+" );
			else
				optionValue.append( "-" );
			optionValue.append( getOptionNameWithoutPrefix( param->getName(), subGroupName ) );
		}
		else
		{
			const std::string optionName( getOptionNameWithoutPrefix( param->getName(), subGroupName ) );
			optionsNameAndValue.insert( OptionForPreset( optionName, boost::to_string( param->getValue() ) ) );
		}
	}

	BOOST_FOREACH( OFX::IntParam* param, _paramInt )
	{
		if( ! subGroupName.empty() && param->getName().find( "_" + subGroupName + "_" ) == std::string::npos )
			continue;
		const std::string optionName( getOptionNameWithoutPrefix( param->getName(), subGroupName ) );
		optionsNameAndValue.insert( OptionForPreset( optionName, boost::to_string( param->getValue() ) ) );
	}

	BOOST_FOREACH( OFX::DoubleParam* param, _paramDouble )
	{
		if( ! subGroupName.empty() && param->getName().find( "_" + subGroupName + "_" ) == std::string::npos )
			continue;
		const std::string optionName( getOptionNameWithoutPrefix( param->getName(), subGroupName ) );
		optionsNameAndValue.insert( OptionForPreset( optionName, boost::to_string( param->getValue() ) ) );
	}

	BOOST_FOREACH( OFX::StringParam* param, _paramString )
	{
		if( ! subGroupName.empty() && param->getName().find( "_" + subGroupName + "_" ) == std::string::npos )
			continue;
		const std::string optionName( getOptionNameWithoutPrefix( param->getName(), subGroupName ) );
		const std::string optionValue( boost::to_string( param->getValue() ) );
		if( ! optionValue.empty() )
			optionsNameAndValue.insert( OptionForPreset( optionName, optionValue ) );
	}

	BOOST_FOREACH( OFX::Int2DParam* param, _paramRatio )
	{
		if( ! subGroupName.empty() && param->getName().find( "_" + subGroupName + "_" ) == std::string::npos )
			continue;
		const std::string optionName( getOptionNameWithoutPrefix( param->getName(), subGroupName ) );
		const std::string optionValue( boost::to_string( param->getValue().x ) + "." + boost::to_string( param->getValue().y ) );
		optionsNameAndValue.insert( OptionForPreset( optionName, optionValue ) );
	}

	BOOST_FOREACH( OFX::ChoiceParam* param, _paramChoice )
	{
		if( ! subGroupName.empty() && param->getName().find( "_" + subGroupName + "_" ) == std::string::npos )
			continue;
//		std::string optionName( getOptionNameWithoutPrefix( param->getName(), subGroupName ) );
//		// @todo: get optionValue from the optionIndex
//		size_t optionIndex = param->getValue();
//		std::string optionValue( "not yet implemented" );
//		optionsNameAndValue.push_back( OptionForPreset( optionName, optionValue ) );
	}

	return optionsNameAndValue;
}

void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, avtranscoder::OptionLoader::OptionArray& optionsArray, const std::string& prefix, const std::string& subGroupName )
{
	OFX::ParamDescriptor* param = NULL;
	BOOST_FOREACH( avtranscoder::Option& option, optionsArray )
	{
		std::string name = prefix;
		if( ! subGroupName.empty() )
		{
			name += subGroupName;
			name += "_";
		}
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
				std::string groupName = prefix;
				groupName += "g_";
				if( ! subGroupName.empty() )
				{
					groupName += subGroupName;
					groupName += "_";
				}
				groupName += option.getName();
				
				OFX::GroupParamDescriptor* groupParam = desc.defineGroupParam( groupName );
				groupParam->setOpen( false );
				BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
				{
					std::string childName = prefix;
					if( ! subGroupName.empty() )
					{
						childName += subGroupName;
						childName += "_";
					}
					childName += child.getUnit();
					childName += "flags_";
					childName += child.getName();
					
					OFX::BooleanParamDescriptor* param = desc.defineBooleanParam( childName );
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

void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group,  avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& prefix )
{
	// iterate on map keys
	BOOST_FOREACH( avtranscoder::OptionLoader::OptionMap::value_type& subGroupOption, optionsMap )
	{
		const std::string subGroupName = subGroupOption.first;
		avtranscoder::OptionLoader::OptionArray& options = subGroupOption.second;
		
		addOptionsToGroup( desc, group, options, prefix, subGroupName );
	}
}

std::string getOptionNameWithoutPrefix( const std::string& optionName, const std::string& subGroupName )
{
	std::string nameWithoutPrefix( optionName );
	
	if( nameWithoutPrefix.find( kPrefixFormat ) != std::string::npos )
		nameWithoutPrefix.erase( 0, kPrefixFormat.size() );
	else if( nameWithoutPrefix.find( kPrefixVideo ) != std::string::npos )
		nameWithoutPrefix.erase( 0, kPrefixVideo.size() );
	else if( nameWithoutPrefix.find( kPrefixAudio ) != std::string::npos )
		nameWithoutPrefix.erase( 0, kPrefixAudio.size() );
	
	// sub group name
	if( ! subGroupName.empty() && nameWithoutPrefix.find( subGroupName ) != std::string::npos )
	{
		// subGroupName.size() + 1: also remove the "_"
		nameWithoutPrefix.erase( 0, subGroupName.size() + 1 );
	}
	
	// childs of groups
	const std::string prefixChild( "flags_" );
	if( nameWithoutPrefix.find( prefixChild ) != std::string::npos )
	{
		// remove xxxflags at the beginning og the string
		size_t endedPosition = nameWithoutPrefix.find( prefixChild );
		nameWithoutPrefix.erase( 0, endedPosition + prefixChild.size() );
	}
	
	return nameWithoutPrefix;
}

}
}
}
}
