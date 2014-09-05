#include "util.hpp"

#include <writer/AVWriterDefinitions.hpp>

#include <AvTranscoder/Option.hpp>

#include <boost/foreach.hpp>

#include <iostream>
#include <limits>

namespace tuttle {
namespace plugin {
namespace av {
namespace common {

CustomParams::OptionsForPreset CustomParams::getOptionsNameAndValue( const std::string& subGroupName ) const
{
	OptionsForPreset optionsNameAndValue;

	BOOST_FOREACH( OFX::BooleanParam* param, _paramBoolean )
	{
		if( ! subGroupName.empty() && param->getName().find( "_" + subGroupName + "_" ) == std::string::npos )
			continue;

		// FFMPEG exception with the flags
		if( param->getName().find( kPrefixFlag ) != std::string::npos )
		{
			std::string optionValue;
			if( param->getValue() )
				optionValue.append( "+" );
			else
				optionValue.append( "-" );
			optionValue.append( getOptionNameWithoutPrefix( param->getName(), subGroupName ) );
			
			const std::string flagName( getOptionFlagName( param->getName(), subGroupName ) );
			// if first flag with this flagName
			if( optionsNameAndValue.find( flagName ) == optionsNameAndValue.end() )
			{
				optionsNameAndValue.insert( OptionForPreset( flagName, optionValue ) );
			}
			// get all flags with the same flagName in a single Option
			else
			{
				optionsNameAndValue.at( flagName ) += optionValue;
			}
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
		std::string optionName( getOptionNameWithoutPrefix( param->getName(), subGroupName ) );
		
		size_t optionIndex = param->getValue();
		ChildList childs( _childsPerChoice.at( param->getName() ) );
		if( childs.size() > 0 )
		{
			std::string optionValue( childs.at( optionIndex ) );
			optionsNameAndValue.insert( OptionForPreset( optionName, optionValue ) );
		}
	}

	return optionsNameAndValue;
}

void CustomParams::fetchCustomParams( OFX::ImageEffect& plugin, avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& prefix )
{
	// iterate on map keys
	BOOST_FOREACH( avtranscoder::OptionLoader::OptionMap::value_type& subGroupOption, optionsMap )
	{
		const std::string subGroupName = subGroupOption.first;
		std::vector<avtranscoder::Option>& options = subGroupOption.second;
				
		fetchCustomParams( plugin, options, prefix, subGroupName );
	}
}

void CustomParams::fetchCustomParams( OFX::ImageEffect& plugin, avtranscoder::OptionLoader::OptionArray& optionsArray, const std::string& prefix, const std::string& subGroupName )
{
	// iterate on options
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
				_paramBoolean.push_back( plugin.fetchBooleanParam( name ) );
				break;
			}
			case avtranscoder::TypeInt:
			{
				_paramInt.push_back( plugin.fetchIntParam( name ) );
				break;
			}
			case avtranscoder::TypeDouble:
			{
				_paramDouble.push_back( plugin.fetchDoubleParam( name ) );
				break;
			}
			case avtranscoder::TypeString:
			{
				_paramString.push_back( plugin.fetchStringParam( name ) );
				break;
			}
			case avtranscoder::TypeRatio:
			{
				_paramRatio.push_back( plugin.fetchInt2DParam( name ) );
				break;
			}
			case avtranscoder::TypeChoice:
			{
				// avoid warning of Host when OFX Choice parameter with no choices
				if( ! option.getNbChilds() )
					continue;

				_paramChoice.push_back( plugin.fetchChoiceParam( name ) );
				_childsPerChoice.insert( common::CustomParams::ChildsForChoice( name, common::CustomParams::ChildList() ) );
				BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
				{
					_childsPerChoice.at( name ).push_back( child.getName() );
				}
				break;
			}
			case avtranscoder::TypeGroup:
			{
				BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
				{
					std::string childName = prefix;
					if( ! subGroupName.empty() )
					{
						childName += subGroupName;
						childName += "_";
					}
					childName += child.getUnit();
					childName += common::kPrefixFlag;
					childName += child.getName();

					_paramBoolean.push_back( plugin.fetchBooleanParam( childName ) );
				}
				break;
			}
		default:
				break;
		}
	}
}

avtranscoder::Profile::ProfileDesc CustomParams::getCorrespondingProfileDesc( const std::string& subGroupName ) const
{
	avtranscoder::Profile::ProfileDesc profileDesc;
	OptionsForPreset optionsForPreset = getOptionsNameAndValue( subGroupName );
	BOOST_FOREACH( OptionsForPreset::value_type& nameAndValue, optionsForPreset )
	{
		profileDesc[ nameAndValue.first ] = nameAndValue.second;
	}
	return profileDesc;
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
				const int min = option.getMin() > std::numeric_limits<int>::min() ? option.getMin() : std::numeric_limits<int>::min();
				const int max = option.getMax() < std::numeric_limits<int>::max() ? option.getMax() : std::numeric_limits<int>::max();
				intParam->setRange( min, max );
				intParam->setDisplayRange( min, max );
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
				// @todo: minX, minY, maxX, maxY could be different
				ratioParam->setDefault( option.getDefaultValueRatio().first, option.getDefaultValueRatio().second );
				const int min = option.getMin() > std::numeric_limits<int>::min() ? option.getMin() : std::numeric_limits<int>::min();
				const int max = option.getMax() < std::numeric_limits<int>::max() ? option.getMax() : std::numeric_limits<int>::max();
				ratioParam->setRange( min, min, max, max );
				ratioParam->setDisplayRange( min, min, max, max );
				param = ratioParam;
				break;
			}
			case avtranscoder::TypeChoice:
			{
				// avoid warning of Host when OFX Choice parameter with no choices
				if( ! option.getNbChilds() )
					continue;

				OFX::ChoiceParamDescriptor* choiceParam = desc.defineChoiceParam( name );
				choiceParam->setDefault( option.getDefaultChildIndex() );
				BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
				{
					choiceParam->appendOption( child.getName() + " " + child.getHelp() );
				}
				param = choiceParam;
				break;
			}
			case avtranscoder::TypeGroup:
			{
				std::string groupName = prefix;
				groupName += kPrefixGroup;
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
					childName += kPrefixFlag;
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
	
	// prefix
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
	
	// childs of groups (flag)
	size_t endedPosition;
	if( ( endedPosition = nameWithoutPrefix.find( kPrefixFlag ) ) != std::string::npos )
	{
		nameWithoutPrefix.erase( 0, endedPosition + kPrefixFlag.size() );
	}
	
	return nameWithoutPrefix;
}


std::string getOptionFlagName( const std::string& optionName, const std::string& subGroupName )
{
	std::string flagName;
	
	if( optionName.find( kPrefixFlag ) != std::string::npos )
	{
		size_t startedPosition;
		if( subGroupName.empty() )
			startedPosition = optionName.find( "_" );
		else
		{
			startedPosition = optionName.find( "_", prefixSize );
		}
		++startedPosition; // started after the "_"
		size_t endedPosition = optionName.find( kPrefixFlag, startedPosition );
		
		flagName = optionName.substr( startedPosition, endedPosition - startedPosition );
	}
	
	return flagName;
}

void disableOFXParamsForFormatOrCodec( OFX::ImageEffect& plugin, avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& filter, const std::string& prefix )
{
	// iterate on map keys
	BOOST_FOREACH( avtranscoder::OptionLoader::OptionMap::value_type& subGroupOption, optionsMap )
	{
		const std::string subGroupName = subGroupOption.first;
		std::vector<avtranscoder::Option>& options = subGroupOption.second;

		// iterate on options
		BOOST_FOREACH( avtranscoder::Option& option, options )
		{
			std::string name = prefix;
			name += subGroupName;
			name += "_";
			name += option.getName();

			switch( option.getType() )
			{
				case avtranscoder::TypeBool:
				{
					OFX::BooleanParam* curOpt = plugin.fetchBooleanParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					break;
				}
				case avtranscoder::TypeInt:
				{
					OFX::IntParam* curOpt = plugin.fetchIntParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					break;
				}
				case avtranscoder::TypeDouble:
				{
					OFX::DoubleParam* curOpt = plugin.fetchDoubleParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					break;
				}
				case avtranscoder::TypeString:
				{
					OFX::StringParam* curOpt = plugin.fetchStringParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					break;
				}
				case avtranscoder::TypeRatio:
				{
					OFX::Int2DParam* curOpt = plugin.fetchInt2DParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					break;
				}
				case avtranscoder::TypeChoice:
				{
					OFX::ChoiceParam* curOpt = plugin.fetchChoiceParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					break;
				}
				case avtranscoder::TypeGroup:
				{
					std::string groupName = prefix;
					groupName += common::kPrefixGroup;
					groupName += subGroupName;
					groupName += "_";
					groupName += option.getName();
					
					OFX::GroupParam* curOpt = plugin.fetchGroupParam( groupName );
					curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					
					BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
					{
						std::string childName = prefix;
						if( ! subGroupName.empty() )
						{
							childName += subGroupName;
							childName += "_";
						}
						childName += child.getUnit();
						childName += common::kPrefixFlag;
						childName += child.getName();
						
						OFX::BooleanParam* curOpt = plugin.fetchBooleanParam( childName );
						curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					}
					break;
				}
				default:
					break;
			}
		}
	}
}

}
}
}
}
