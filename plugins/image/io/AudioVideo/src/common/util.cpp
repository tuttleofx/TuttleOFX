#include "util.hpp"

#include <writer/AVWriterDefinitions.hpp>

#include <AvTranscoder/option/FormatContext.hpp>
#include <AvTranscoder/option/CodecContext.hpp>
#include <AvTranscoder/option/Option.hpp>

#include <boost/foreach.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <iostream>
#include <limits>

namespace tuttle {
namespace plugin {
namespace av {
namespace common {

LibAVParams::LibAVParams( const std::string& prefixScope, const std::string& prefixOperation )
	: _paramOFX()
	, _childsPerChoice()
	, _avContext( NULL )
	{
	    // Create context
	    if( prefixScope == kPrefixFormat )
		{
			if( prefixOperation == kPrefixEncoding )
				_avContext.reset( new avtranscoder::FormatContext( AV_OPT_FLAG_ENCODING_PARAM ) ); 
			else if( prefixOperation == kPrefixDecoding )
				_avContext.reset( new avtranscoder::FormatContext( AV_OPT_FLAG_DECODING_PARAM ) ); 
		}
	    else if( prefixScope == kPrefixVideo )
		{
			if( prefixOperation == kPrefixEncoding )
				_avContext.reset( new avtranscoder::CodecContext( AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM ) );
			else if( prefixOperation == kPrefixDecoding )
				_avContext.reset( new avtranscoder::CodecContext( AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_VIDEO_PARAM ) );
		}
	    else if( prefixScope == kPrefixAudio )
		{
			if( prefixOperation == kPrefixEncoding )
				_avContext.reset( new avtranscoder::CodecContext( AV_OPT_FLAG_ENCODING_PARAM | AV_OPT_FLAG_AUDIO_PARAM ) );
			else if( prefixOperation == kPrefixDecoding )
				_avContext.reset( new avtranscoder::CodecContext( AV_OPT_FLAG_DECODING_PARAM | AV_OPT_FLAG_AUDIO_PARAM ) );
		}
		else if( prefixScope == kPrefixMetaData )
		{
			_avContext.reset( new avtranscoder::FormatContext( AV_OPT_FLAG_METADATA ) ); 
		}
	    else
	    {
			BOOST_THROW_EXCEPTION( exception::Failed()
				<< exception::user() + ": Can't create a context with the given scope " + prefixScope + " and the given operation " + prefixOperation );
	    }
	}

LibAVParams::LibAVOptions LibAVParams::getLibAVOptions( const std::string& subGroupName ) const
{
	LibAVOptions optionsNameAndValue;

	BOOST_FOREACH( OFX::ValueParam* param, _paramOFX )
	{
		if( ! subGroupName.empty() && param->getName().find( "_" + subGroupName + "_" ) == std::string::npos )
			continue;

		const std::string libavOptionName( getOptionNameWithoutPrefix( param->getName(), subGroupName ) );
		std::string optionValue( "" );

		// Manage OFX Boolean
		OFX::BooleanParam* paramBoolean = dynamic_cast<OFX::BooleanParam*>( param );
		if( paramBoolean )
		{
			// FFMPEG flags
			if( param->getName().find( kPrefixFlag ) != std::string::npos )
			{
				std::string optionValue;
				if( paramBoolean->getValue() )
					optionValue.append( "+" );
				else
					optionValue.append( "-" );
				optionValue.append( getOptionNameWithoutPrefix( param->getName(), subGroupName ) );

				const std::string flagName( getOptionFlagName( param->getName(), subGroupName ) );
				// if first flag with this flagName
				if( optionsNameAndValue.find( flagName ) == optionsNameAndValue.end() )
				{
					optionsNameAndValue.insert( std::make_pair( flagName, optionValue ) );
				}
				// get all flags with the same flagName in a single Option
				else
				{
					optionsNameAndValue.at( flagName ) += optionValue;
				}
			}
			else
			{
				optionValue = boost::to_string( paramBoolean->getValue() );
				optionsNameAndValue.insert( std::make_pair( libavOptionName, optionValue ) );
			}
			continue;
		}

		// Manage OFX Int
		OFX::IntParam* paramInt = dynamic_cast<OFX::IntParam*>( param );
		if( paramInt )
		{
			optionValue = boost::to_string( paramInt->getValue() );
			optionsNameAndValue.insert( std::make_pair( libavOptionName, optionValue ) );
			continue;
		}

		// Manage OFX Double
		OFX::DoubleParam* paramDouble = dynamic_cast<OFX::DoubleParam*>( param );
		if( paramDouble )
		{
			optionValue = boost::to_string( paramDouble->getValue() );
			optionsNameAndValue.insert( std::make_pair( libavOptionName, optionValue ) );
			continue;
		}

		// Manage OFX String
		OFX::StringParam* paramString = dynamic_cast<OFX::StringParam*>( param );
		if( paramString )
		{
			optionValue = paramString->getValue();
			if( ! optionValue.empty() )
				optionsNameAndValue.insert( std::make_pair( libavOptionName, optionValue ) );
			continue;
		}

		// Manage OFX Int2D
		OFX::Int2DParam* paramRatio = dynamic_cast<OFX::Int2DParam*>( param );
		if( paramRatio )
		{
			optionValue = boost::to_string( paramRatio->getValue().x ) + ":" + boost::to_string( paramRatio->getValue().y );
			optionsNameAndValue.insert( std::make_pair( libavOptionName, optionValue ) );
			continue;
		}

		// Manage OFX Choice
		OFX::ChoiceParam* paramChoice = dynamic_cast<OFX::ChoiceParam*>( param );
		if( paramChoice )
		{
			size_t optionIndex = paramChoice->getValue();
			std::vector<std::string> childs( _childsPerChoice.at( paramChoice ) );
			if( childs.size() > optionIndex )
			{
				optionValue = childs.at( optionIndex );
				optionsNameAndValue.insert( std::make_pair( libavOptionName, optionValue ) );
			}
			continue;
		}
	}
	return optionsNameAndValue;
}

void LibAVParams::fetchLibAVParams( OFX::ImageEffect& plugin, avtranscoder::OptionArrayMap& optionArrayMap, const std::string& prefix )
{
	// iterate on map keys
	BOOST_FOREACH( avtranscoder::OptionArrayMap::value_type& subGroupOption, optionArrayMap )
	{
		const std::string subGroupName = subGroupOption.first;
		avtranscoder::OptionArray& options = subGroupOption.second;
				
		fetchLibAVParams( plugin, options, prefix, subGroupName );
	}
}

void LibAVParams::fetchLibAVParams( OFX::ImageEffect& plugin, avtranscoder::OptionArray& optionsArray, const std::string& prefix, const std::string& subGroupName )
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
			case avtranscoder::eOptionBaseTypeBool:
			{
				_paramOFX.push_back( plugin.fetchBooleanParam( name ) );
				break;
			}
			case avtranscoder::eOptionBaseTypeInt:
			{
				_paramOFX.push_back( plugin.fetchIntParam( name ) );
				break;
			}
			case avtranscoder::eOptionBaseTypeDouble:
			{
				_paramOFX.push_back( plugin.fetchDoubleParam( name ) );
				break;
			}
			case avtranscoder::eOptionBaseTypeString:
			{
				_paramOFX.push_back( plugin.fetchStringParam( name ) );
				break;
			}
			case avtranscoder::eOptionBaseTypeRatio:
			{
				_paramOFX.push_back( plugin.fetchInt2DParam( name ) );
				break;
			}
			case avtranscoder::eOptionBaseTypeChoice:
			{
				// avoid warning of Host when OFX Choice parameter with no choices
				if( option.getChilds().empty() )
					continue;

				OFX::ChoiceParam* choice = plugin.fetchChoiceParam( name );
				_paramOFX.push_back( choice );
				_childsPerChoice.insert( std::make_pair( choice, std::vector<std::string>() ) );
				BOOST_FOREACH( const avtranscoder::Option& child, option.getChilds() )
				{
					_childsPerChoice.at( choice ).push_back( child.getName() );
				}
				break;
			}
			case avtranscoder::eOptionBaseTypeGroup:
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

					_paramOFX.push_back( plugin.fetchBooleanParam( childName ) );
				}
				break;
			}
		default:
				break;
		}
	}
}

avtranscoder::ProfileLoader::Profile LibAVParams::getCorrespondingProfile( const std::string& subGroupName ) const
{
	avtranscoder::ProfileLoader::Profile profile;
	LibAVOptions optionsForPreset = getLibAVOptions( subGroupName );
	BOOST_FOREACH( LibAVOptions::value_type& nameAndValue, optionsForPreset )
	{
		try
		{
			// Skip options with a current value equals to their default value
			const avtranscoder::Option& option = _avContext->getOption( nameAndValue.first );
			switch( option.getType() )
			{
				case avtranscoder::eOptionBaseTypeBool:
				{
					if( option.getDefaultBool() == option.getBool() )
						continue;
					break;
				}
				case avtranscoder::eOptionBaseTypeInt:
				{
					int defaultValue = option.getDefaultInt();
					int value = option.getInt();
					if( option.getDefaultInt() == option.getInt() )
						continue;
					break;
				}
				case avtranscoder::eOptionBaseTypeDouble:
				{
					if( boost::test_tools::check_is_close( option.getDefaultDouble(), option.getDouble(), boost::test_tools::percent_tolerance( 0.5 ) ) )
						continue;
					break;
				}
				case avtranscoder::eOptionBaseTypeString:
				{
					if( option.getDefaultString() == option.getString() )
						continue;
					break;
				}
				case avtranscoder::eOptionBaseTypeRatio:
				{
					if( option.getDefaultRatio() == option.getRatio() )
						continue;
					break;
				}
				case avtranscoder::eOptionBaseTypeChoice:
				{
					if( option.getDefaultInt() == option.getInt() )
						continue;
					break;
				}
				default:
					continue;
			}
			profile[ nameAndValue.first ] = nameAndValue.second;
		}
		catch( std::exception& e )
		{
			TUTTLE_TLOG( TUTTLE_INFO, "Can't get option " << nameAndValue.first << ": " << e.what() );
		}
	}
	return profile;
}

void LibAVParams::setOption( const std::string& libAVOptionName, const std::string& value,  const std::string& prefix, const std::string& subGroupName )
{
	try
	{
		// Get option from context
		avtranscoder::Option& option = _avContext->getOption( libAVOptionName );

		// Set FFmpeg option's value
		option.setString( value );

		// Get corresponding OFX parameter
		OFX::ValueParam* param = getOFXParameter( libAVOptionName, subGroupName );
		if( ! param)
		{
			TUTTLE_TLOG( TUTTLE_INFO, "Can't get OFX parameter corresponding to option " << libAVOptionName << " of subgroup " << subGroupName << ": " << e.what() );
		}

		// Set OFX parameter's value
		OFX::BooleanParam* paramBoolean = dynamic_cast<OFX::BooleanParam*>( param );
		if( paramBoolean )
		{
			paramBoolean->setValue( option.getBool() );
			return;
		}
		OFX::IntParam* paramInt = dynamic_cast<OFX::IntParam*>( param );
		if( paramInt )
		{
			paramInt->setValue( option.getInt() );
			return;
		}
		OFX::DoubleParam* paramDouble = dynamic_cast<OFX::DoubleParam*>( param );
		if( paramDouble )
		{
			paramDouble->setValue( option.getDouble() );
			return;
		}
		OFX::StringParam* paramString = dynamic_cast<OFX::StringParam*>( param );
		if( paramString )
		{
			paramString->setValue( option.getString() );
			return;
		}
		OFX::Int2DParam* paramRatio = dynamic_cast<OFX::Int2DParam*>( param );
		if( paramRatio )
		{
			paramRatio->setValue( option.getRatio().first, option.getRatio().second );
			return;
		}
		OFX::ChoiceParam* paramChoice = dynamic_cast<OFX::ChoiceParam*>( param );
		if( paramChoice )
		{
			paramChoice->setValue( option.getInt() );
			return;
		}
	}
	catch( std::exception& e )
	{
		TUTTLE_TLOG( TUTTLE_INFO, "Can't set option " << libAVOptionName << " to " << value << ": " << e.what() );
	}
}

OFX::ValueParam* LibAVParams::getOFXParameter( const std::string& libAVOptionName, const std::string& subGroupName )
{
	BOOST_FOREACH( OFX::ValueParam* param, _paramOFX )
	{
		if( getOptionNameWithoutPrefix( param->getName(), subGroupName ) == libAVOptionName )
			return param;
	}
	return NULL;
}

void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group, avtranscoder::OptionArray& optionsArray, const std::string& prefix, const std::string& subGroupName )
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
			case avtranscoder::eOptionBaseTypeBool:
			{
				OFX::BooleanParamDescriptor* boolParam = desc.defineBooleanParam( name );
				boolParam->setDefault( option.getDefaultBool() );
				param = boolParam;
				break;
			}
			case avtranscoder::eOptionBaseTypeInt:
			{
				OFX::IntParamDescriptor* intParam = desc.defineIntParam( name );
				intParam->setDefault( option.getDefaultInt() );
				const int min = option.getMin() > std::numeric_limits<int>::min() ? option.getMin() : std::numeric_limits<int>::min();
				const int max = option.getMax() < std::numeric_limits<int>::max() ? option.getMax() : std::numeric_limits<int>::max();
				intParam->setRange( min, max );
				intParam->setDisplayRange( min, max );
				param = intParam;
				break;
			}
			case avtranscoder::eOptionBaseTypeDouble:
			{
				OFX::DoubleParamDescriptor* doubleParam = desc.defineDoubleParam( name );
				doubleParam->setDefault( option.getDefaultDouble() );
				doubleParam->setRange( option.getMin(), option.getMax() );
				doubleParam->setDisplayRange( option.getMin(), option.getMax() );
				param = doubleParam;
				break;
			}
			case avtranscoder::eOptionBaseTypeString:
			{
				OFX::StringParamDescriptor* strParam = desc.defineStringParam( name );
				strParam->setDefault( option.getDefaultString() );
				param = strParam;
				break;
			}
			case avtranscoder::eOptionBaseTypeRatio:
			{
				OFX::Int2DParamDescriptor* ratioParam = desc.defineInt2DParam( name );
				// @todo: minX, minY, maxX, maxY could be different
				ratioParam->setDefault( option.getDefaultRatio().first, option.getDefaultRatio().second );
				const int min = option.getMin() > std::numeric_limits<int>::min() ? option.getMin() : std::numeric_limits<int>::min();
				const int max = option.getMax() < std::numeric_limits<int>::max() ? option.getMax() : std::numeric_limits<int>::max();
				ratioParam->setRange( min, min, max, max );
				ratioParam->setDisplayRange( min, min, max, max );
				param = ratioParam;
				break;
			}
			case avtranscoder::eOptionBaseTypeChoice:
			{
				// avoid warning of Host when OFX Choice parameter with no choices
				if( option.getChilds().empty() )
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
			case avtranscoder::eOptionBaseTypeGroup:
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

void addOptionsToGroup( OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group,  avtranscoder::OptionArrayMap& optionArrayMap, const std::string& prefix )
{
	// iterate on map keys
	BOOST_FOREACH( avtranscoder::OptionArrayMap::value_type& subGroupOption, optionArrayMap )
	{
		const std::string subGroupName = subGroupOption.first;
		avtranscoder::OptionArray& options = subGroupOption.second;
		
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

void disableOFXParamsForFormatOrCodec( OFX::ImageEffect& plugin, avtranscoder::OptionArrayMap& optionArrayMap, const std::string& filter, const std::string& prefix )
{
	// iterate on map keys
	BOOST_FOREACH( avtranscoder::OptionArrayMap::value_type& subGroupOption, optionArrayMap )
	{
		const std::string subGroupName = subGroupOption.first;
		avtranscoder::OptionArray& options = subGroupOption.second;

		// iterate on options
		BOOST_FOREACH( avtranscoder::Option& option, options )
		{
			std::string name = prefix;
			name += subGroupName;
			name += "_";
			name += option.getName();

			switch( option.getType() )
			{
				case avtranscoder::eOptionBaseTypeBool:
				{
					OFX::BooleanParam* curOpt = plugin.fetchBooleanParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					break;
				}
				case avtranscoder::eOptionBaseTypeInt:
				{
					OFX::IntParam* curOpt = plugin.fetchIntParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					break;
				}
				case avtranscoder::eOptionBaseTypeDouble:
				{
					OFX::DoubleParam* curOpt = plugin.fetchDoubleParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					break;
				}
				case avtranscoder::eOptionBaseTypeString:
				{
					OFX::StringParam* curOpt = plugin.fetchStringParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					break;
				}
				case avtranscoder::eOptionBaseTypeRatio:
				{
					OFX::Int2DParam* curOpt = plugin.fetchInt2DParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					break;
				}
				case avtranscoder::eOptionBaseTypeChoice:
				{
					OFX::ChoiceParam* curOpt = plugin.fetchChoiceParam( name );
					curOpt->setIsSecretAndDisabled( !( subGroupName == filter ) );
					break;
				}
				case avtranscoder::eOptionBaseTypeGroup:
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
