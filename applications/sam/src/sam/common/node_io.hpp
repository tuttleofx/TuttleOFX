#ifndef _SAM_NODE_IO_HPP_
#define	_SAM_NODE_IO_HPP_

#include <tuttle/host/Graph.hpp>

#include <vector>
#include <string>

namespace sam {

namespace ttl = tuttle::host;
Color _color;

/// get Default or choice values
std::vector<std::string> getDefaultOrChoiceValues( const tuttle::host::ofx::property::OfxhProperty& prop )
{
    std::vector<std::string> s;
    if( !(prop.getType() == 3) ) // if Pointer, we don't have _value
    {
	int n = 0;
	for( ; n < (int)( prop.getDimension() ) - 1; ++n )
	{
	    s.push_back( prop.getStringValue( n ) );
	}
	if( prop.getDimension() > 0 )
	{
	    s .push_back( prop.getStringValue( n ) );
	}
    }
    return s;
}

/// get defaults values of plugin properties
std::string getPropertyType( const tuttle::host::ofx::property::OfxhProperty& prop )
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


/**
 * @todo
 */
void coutProperties( const ttl::Graph::Node& node )
{
    const ttl::ofx::property::OfxhSet& props = node.getProperties();
    BOOST_FOREACH( ttl::ofx::property::PropertyMap::const_reference clip, props.getMap() )
    {
	TUTTLE_COUT(
	    "\t" <<
	    _color._green <<
	    clip.first << " " <<
	    _color._red <<
	    (clip.second->getDimension() > 1 ? (std::string(" x") + boost::lexical_cast<std::string>(clip.second->getDimension())) : "") <<
	    _color._std
	    );
    }
}

/**
 * @todo
 */
void coutClips( const ttl::Graph::Node& node )
{
//	ttl::ofx::attribute::OfxhClipSet& clips = node.getClipSet();
//	BOOST_FOREACH( ttl::ofx::attribute::OfxhClip& clip, clips.getClipVector() )
//	{
//		/// @todo
//		TUTTLE_COUT("");
//	}
}
/*
void coutParametersWithDetails( const ttl::Graph::Node& node )
{
    const ttl::ofx::attribute::OfxhParamSet& params = node.getParamSet();
    BOOST_FOREACH( const ttl::ofx::attribute::OfxhParam& param, params.getParamVector() )
    {
	if( param.getSecret() )
	    continue; // ignore secret parameters
	TUTTLE_COUT(
	    "\t" <<
	    _color._green <<
	    param.getScriptName() << ":\t" <<
	    _color._red <<
	    param.getParamType() <<
	    (param.getSize() > 1 ? (std::string(" x") + boost::lexical_cast<std::string>(param.getSize())) : "") <<
	    _color._std
	    );
	const std::string& hint = param.getHint();
	if( hint.size() )
	{
	    TUTTLE_COUT( hint );
	}
	TUTTLE_COUT("");
    }
}*/

void coutParametersWithDetails( const tuttle::host::ofx::property::OfxhSet properties, std::string context="" )
{
	std::vector<std::string> defaultValue;
	std::vector<std::string> choiceValues;

	tuttle::host::ofx::property::PropertyMap propMap = properties.getMap();
	for( tuttle::host::ofx::property::PropertyMap::const_iterator itProperty = propMap.begin(); itProperty != propMap.end(); ++itProperty )
	{
		const tuttle::host::ofx::property::OfxhProperty& prop = *( itProperty->second );

		std::string label = itProperty->first;

		if( std::strcmp( label.c_str() , kOfxParamPropChoiceOption ) == 0 )
		{
			choiceValues = getDefaultOrChoiceValues( prop );
		}
		if( std::strcmp( label.c_str() , kOfxParamPropDefault ) == 0 )
		{
			defaultValue = getDefaultOrChoiceValues( prop );
		}
		if( std::strcmp( label.c_str() , kOfxParamPropType ) == 0 )
		{
			const std::string type = getPropertyType( prop ).substr( 12 ); // remove ofx prefix "OfxParamProp"

			if( std::strcmp( type.c_str() , "Group" ) && std::strcmp( type.c_str() , "PushButton" )  ) // if it isn't a group or a button parameter, we print the parameter.
			{
				std::string stringDefaultValue;
				for (unsigned int i=0; i<defaultValue.size(); i++ )
					stringDefaultValue += defaultValue.at(i) + ",";
				stringDefaultValue.erase( stringDefaultValue.size()-1, 1 );

				if( choiceValues.size() ) // if it's a choice, we take the nth argument
				{
					int indexOfDefaultValue = boost::lexical_cast<int>( defaultValue.at(0).c_str() );
					stringDefaultValue = choiceValues.at( indexOfDefaultValue ) ;
					TUTTLE_COUT( "\t" << _color._green << std::left << std::setw (25) << context + ":" << _color._yellow << std::setw( 15 ) << stringDefaultValue << _color._std );
					for( size_t i=0; i < choiceValues.size(); i++ )
						TUTTLE_COUT( "\t\t\t\t\t" << sam::_color._red << "- " << choiceValues.at( i ) << _color._std );
				}
				else
				{
					TUTTLE_COUT( "\t" << _color._green << std::left << std::setw (25) << context + ":" << _color._std << std::setw( 15 ) << type << sam::_color._yellow << stringDefaultValue << _color._std );
				}
			}
		}
	}
}

void coutParametersWithDetails( const ttl::Graph::Node& node )
{
	const ttl::ofx::attribute::OfxhParamSet& params = node.getParamSet();

	BOOST_FOREACH( const ttl::ofx::attribute::OfxhParam& param, params.getParamVector() )
	{
		//if( param.getSecret() )
		//    continue; // ignore secret parameters
		const std::string ofxType = param.getParamTypeName();

		if( std::strcmp( ofxType.c_str() , "Group" ) ) // if it isn't a group parameter, we print the parameter.
			coutParametersWithDetails( param.getProperties(), param.getScriptName() );

		//TUTTLE_COUT( _color._green << "[ " << strParamsContexts << " ]" << _color._std );
	}
}

void coutParameters( const ttl::Graph::Node& node )
{
    const ttl::ofx::attribute::OfxhParamSet& params = node.getParamSet();
    BOOST_FOREACH( const ttl::ofx::attribute::OfxhParam& param, params.getParamVector() )
    {
	//if( param.getSecret() )
	//    continue; // ignore secret parameters
	TUTTLE_COUT( param.getScriptName() );
    }
}

}

#endif
