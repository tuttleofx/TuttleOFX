#ifndef _SAM_PROPERTIES_HPP_
#define _SAM_PROPERTIES_HPP_

#include "node_io.hpp"
#include <tuttle/host/Graph.hpp>

/// get defaults values of plugin properties
std::string getPropertyType(const tuttle::host::ofx::property::OfxhProperty& prop)
{
	std::string s;
	if( !(prop.getType() == 3) ) // if Pointer, we don't have _value and _defaultValue properties
	{
		int n = 0;
		if( prop.getDimension() > 0 )
		{
			s += prop.getStringValue( n );
		}
	}
	return s;
}

void printProperties( const tuttle::host::ofx::property::OfxhSet properties, std::string context="" )
{
	std::vector<std::string> defaultValue;
	std::vector<std::string> choiceValues;

	tuttle::host::ofx::property::PropertyMap propMap = properties.getMap();
	for( tuttle::host::ofx::property::PropertyMap::const_iterator itProperty = propMap.begin(); itProperty != propMap.end(); ++itProperty )
	{
		const tuttle::host::ofx::property::OfxhProperty& prop = *( itProperty->second );

		std::string label = itProperty->first;

		if( std::strcmp( label.c_str() , "OfxParamPropChoiceOption" ) == 0 )
		{
		choiceValues = sam::getDefaultOrChoiceValues( prop );
		}
		if( std::strcmp( label.c_str() , "OfxParamPropDefault" ) == 0 )
		{
			defaultValue = sam::getDefaultOrChoiceValues( prop );
		}
		if( std::strcmp( label.c_str() , "OfxParamPropType" ) == 0 )
		{
			std::string type = getPropertyType( prop );
			type.erase(0, 12);
			if( std::strcmp( type.c_str() , "Group" ) ) // if it isn't a group parameter, we print the parameter.
			{
				std::string stringDefaultValue;
				for (unsigned int i=0; i<defaultValue.size(); i++ )
					stringDefaultValue += defaultValue.at(i) + ",";
				stringDefaultValue.erase( stringDefaultValue.size()-1, 1 );

				if( choiceValues.size() ) // if it's a choice, we take the nth argument
				{
					int indexOfDefaultValue = boost::lexical_cast<int>( defaultValue.at(0).c_str() );
					stringDefaultValue = choiceValues.at( indexOfDefaultValue ) ;
					TUTTLE_COUT( "\t" << sam::_color._green << std::left << std::setw (25) << context + ":" << sam::_color._yellow << std::setw( 15 ) << stringDefaultValue << sam::_color._std );
					for( size_t i=0; i < choiceValues.size(); i++ )
						TUTTLE_COUT( "\t\t\t\t\t" << sam::_color._red << "- " << choiceValues.at( i ) << sam::_color._std );
				}
				else
				{
					TUTTLE_COUT( "\t" << sam::_color._green << std::left << std::setw (25) << context + ":" << sam::_color._std << std::setw( 15 ) << type << sam::_color._yellow << stringDefaultValue << sam::_color._std );
				}
			}
		}
	}
}


#endif
