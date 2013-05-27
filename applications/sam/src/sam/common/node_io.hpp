#ifndef _SAM_NODE_IO_HPP_
#define	_SAM_NODE_IO_HPP_

#include <tuttle/host/Graph.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>

#include <extensions/tuttle/ofxParam.h>

#include <vector>
#include <string>

namespace ttl = ::tuttle::host;

namespace sam {

/**
 * @brief Get all values of the property as a vector of strings.
 */
std::vector<std::string> getStringValues( const tuttle::host::ofx::property::OfxhProperty& prop )
{
	std::vector<std::string> s;
	for( std::size_t n = 0; n < prop.getDimension(); ++n )
	{
		s.push_back( prop.getStringValue( n ) );
	}
	return s;
}

/**
 * @brief Return a string to represent the property value or values.
 * If multiple values, return a string like "a,b,c".
 */
std::string getFormattedStringValue( const tuttle::host::ofx::property::OfxhProperty& prop )
{
	return boost::algorithm::join( getStringValues( prop ), "," );
}

/**
 * @todo
 */
void coutProperties( const ttl::Graph::Node& node )
{
	const ttl::ofx::property::OfxhSet& props = node.getProperties();
	boost::shared_ptr<tuttle::common::Color>  color( tuttle::common::Color::get() );
	
	BOOST_FOREACH( ttl::ofx::property::PropertyMap::const_reference clip, props.getMap() )
	{
		TUTTLE_LOG_INFO(
					"\t" <<
					color->_green <<
					clip.first << " " <<
					color->_red <<
					( clip.second->getDimension() > 1 ? ( std::string( " x" ) + boost::lexical_cast<std::string > ( clip.second->getDimension() ) ) : "" ) <<
					color->_std
					);
	}
}

/**
 * @todo
 */
void coutClipsWithDetails( const ttl::Graph::Node& node )
{
	const ttl::ofx::attribute::OfxhClipImageSet& clips = node.getClipImageSet();
	boost::shared_ptr<tuttle::common::Color>  color( tuttle::common::Color::get() );
	
	BOOST_FOREACH( const ttl::ofx::attribute::OfxhClipImage& clip, clips.getClipsByOrder() )
	{
		std::stringstream stream;
		
		stream << "\t" << color->_green << std::left << std::setw( 25 ) << clip.getName() << ": " << color->_std;
		
		stream << color->_yellow;
		{
			std::vector<std::string> components = clip.getSupportedComponents();
			BOOST_FOREACH( std::string& s, components )
			{
				s = s.substr( 17 ); // remove 'OfxImageComponent'
			}
			stream << "[" << boost::algorithm::join( components, ", " ) << "]";
		}
		if( clip.getProperties().getIntProperty( kOfxImageClipPropOptional ) )
		{
			stream << ", optional";
		}
		if( clip.isMask() )
		{
			stream << ", mask";
		}
//		if( clip.supportsTiles() )
//		{
//			stream << " supportTiles";
//		}
		if( clip.temporalAccess() )
		{
			stream << ", use temporal access";
		}
		stream << color->_std;
		TUTTLE_LOG_INFO( stream.str() );
	}
}

void coutClips( const ttl::Graph::Node& node )
{
	const ttl::ofx::attribute::OfxhClipImageSet& clips = node.getClipImageSet();
	BOOST_FOREACH( const ttl::ofx::attribute::OfxhClipImage& clip, clips.getClipsByOrder() )
	{
		TUTTLE_LOG_INFO( clip.getName() );
	}
}

void coutParameterValues( std::ostream& os, const ttl::ofx::attribute::OfxhParam& param )
{
	using namespace tuttle::host::ofx::property;
	
	if( param.getProperties().hasProperty( kOfxParamPropChoiceOption ) )
	{
		const OfxhProperty& prop = param.getProperties().fetchProperty( kOfxParamPropChoiceOption );
		
		for( std::size_t n = 0; n < prop.getDimension(); ++n )
		{
			os << prop.getStringValue( n ) << " ";
		}
	}
	else
	{
		const OfxhProperty& prop = param.getProperties().fetchProperty( kOfxParamPropDefault );
		const std::string defaultValue = getFormattedStringValue( prop );
		
		if( ! param.getProperties().hasProperty( kOfxParamPropDisplayMin ) )
		{
			// no min/max values, we are not a numeric parameter
			os << defaultValue << " ";
		}
		else
		{
//			const OfxhProperty& propMin = param.getProperties().fetchProperty( kOfxParamPropMin );
//			const std::string minValue = getFormattedStringValue( propMin );
			const OfxhProperty& propDisplayMin = param.getProperties().fetchProperty( kOfxParamPropDisplayMin );
			const std::string displayMinValue = getFormattedStringValue( propDisplayMin );
			
//			os << minValue << " ";
//			if( minValue != displayMinValue )
				os << displayMinValue << " ";
			
			os << defaultValue << " ";
			
			const OfxhProperty& propDisplayMax = param.getProperties().fetchProperty( kOfxParamPropDisplayMax );
			const std::string displayMaxValue = getFormattedStringValue( propDisplayMax );
//			const OfxhProperty& propMax = param.getProperties().fetchProperty( kOfxParamPropMax );
//			const std::string maxValue = getFormattedStringValue( propMax );
			
			os << displayMaxValue << " ";
//			if( minValue != displayMinValue )
//				os << maxValue << " ";
		}
		
	}
}

/*
void coutParametersWithDetails( const ttl::Graph::Node& node )
{
	const ttl::ofx::attribute::OfxhParamSet& params = node.getParamSet();
	BOOST_FOREACH( const ttl::ofx::attribute::OfxhParam& param, params.getParamVector() )
	{
	if( param.getSecret() )
		continue; // ignore secret parameters
	TUTTLE_LOG_INFO(
		"\t" <<
		color->_green <<
		param.getScriptName() << ":\t" <<
		color->_red <<
		param.getParamType() <<
		(param.getSize() > 1 ? (std::string(" x") + boost::lexical_cast<std::string>(param.getSize())) : "") <<
		color->_std
		);
	const std::string& hint = param.getHint();
	if( hint.size() )
	{
		TUTTLE_LOG_INFO( hint );
	}
	TUTTLE_LOG_INFO("");
	}
}*/

void coutParameterWithDetails( const ttl::ofx::attribute::OfxhParam& param )
{
	using namespace tuttle::host::ofx::property;
	boost::shared_ptr<tuttle::common::Color>  color( tuttle::common::Color::get() );
	
	const std::string type = param.getParamType();
	const std::string typeName = param.getParamTypeName();
	
	// don't print: group, button, page parameters
	if( ( type == kOfxParamTypeGroup ) ||
		( type == kOfxParamTypePushButton ) ||
		( type == kOfxParamTypePage ) )
		return;
	
	std::vector<std::string> defaultValues;
	int choiceDefaultIndexValue = 0;
	
	if( param.getProperties().hasProperty( kOfxParamPropDefault ) )
	{
		const OfxhProperty& prop = param.getProperties().fetchProperty( kOfxParamPropDefault );
		defaultValues = getStringValues( prop );
		if( prop.getType() == ePropTypeInt )
		{
			choiceDefaultIndexValue = param.getProperties().getIntProperty( kOfxParamPropDefault, 0 );
		}
	}
	
	if( type == kOfxParamTypeChoice )
	{
		std::vector<std::string> choiceValues;
		std::vector<std::string> choiceLabelValues;
		bool hasLabel = false;
		if( param.getProperties().hasProperty( kOfxParamPropChoiceOption ) )
		{
			const OfxhProperty& prop = param.getProperties().fetchProperty( kOfxParamPropChoiceOption );
			choiceValues = getStringValues( prop );
		}
		if( param.getProperties().hasProperty( kOfxParamPropChoiceLabelOption ) )
		{
			const OfxhProperty& prop = param.getProperties().fetchProperty( kOfxParamPropChoiceLabelOption );
			choiceLabelValues = getStringValues( prop );
			hasLabel = ( choiceValues.size() == choiceLabelValues.size() );
		}
		if( choiceValues.size() ) // if it's a choice, we take the nth argument
		{
			static const std::string standardItem = " -  ";
			static const std::string defaultItem  = "--> ";
			
			std::stringstream stream;
			stream << "\t" << color->_green << std::left << std::setw( 25 ) << param.getScriptName() << ": " <<
					  color->_std << std::setw( 15 ) << typeName << color->_std;
			
			TUTTLE_LOG_INFO( stream.str() );
			for( std::size_t i = 0; i < choiceValues.size(); ++i )
			{
				std::stringstream stream;
				stream << "\t\t\t\t\t";
				if( (std::size_t)(choiceDefaultIndexValue) == i )
				{
					stream << color->_yellow << defaultItem;
				}
				else
				{
					stream << color->_red << standardItem;
				}
				stream << std::left << std::setw( 20 ) << choiceValues[i];
				if( hasLabel && ! choiceLabelValues[i].empty() )
				{
					stream << "\"" << choiceLabelValues[i] << "\"";
				}
				stream << color->_std;
				TUTTLE_LOG_INFO( stream.str() );
			}
		}
	}
	else
	{
		std::stringstream stream;
		const std::string stringDefaultValue =  boost::algorithm::join( defaultValues, "," );
		stream << "\t" <<
			color->_green << std::left << std::setw( 25 ) << param.getScriptName() << ": " <<
			color->_std << std::setw( 15 ) << typeName << color->_yellow;
		
		stream <<  boost::algorithm::join( defaultValues, "," ) << color->_std;

		if( param.getProperties().hasProperty( kOfxParamPropDisplayMin ) )
		{
			const OfxhProperty& propDisplayMin = param.getProperties().fetchProperty( kOfxParamPropDisplayMin );
			const OfxhProperty& propDisplayMax = param.getProperties().fetchProperty( kOfxParamPropDisplayMax );
			const std::string minDisplayValue = getFormattedStringValue( propDisplayMin );
			const std::string maxDisplayValue = getFormattedStringValue( propDisplayMax );

			stream << "  [" << minDisplayValue << " --> " << maxDisplayValue << "]";
		}
		TUTTLE_LOG_INFO( stream.str() );
	}
	
	if( param.getProperties().hasProperty( kOfxParamPropHint ) )
	{
		const std::string& hint = param.getProperties().getStringProperty( kOfxParamPropHint );
		if( hint.size() )
		{
			std::vector<std::string> hintSplitted;
			boost::split( hintSplitted, hint, boost::is_any_of("\n"));
			BOOST_FOREACH( const std::string hintString, hintSplitted )
			{
				std::stringstream stream;
				stream << std::left << std::setw( 10 ) << " " << hintString;
				TUTTLE_LOG_INFO( stream.str() );
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
		
		// if it is a displayable parameter
		coutParameterWithDetails( param );
		//TUTTLE_LOG_TRACE( color->_green << "[ " << strParamsContexts << " ]" << color->_std );
	}
}

void coutParameters( const ttl::Graph::Node& node )
{
	const ttl::ofx::attribute::OfxhParamSet& params = node.getParamSet();

	BOOST_FOREACH( const ttl::ofx::attribute::OfxhParam& param, params.getParamVector() )
	{
		//if( param.getSecret() )
		//    continue; // ignore secret parameters
		TUTTLE_LOG_INFO( param.getScriptName() );
	}
}

}

#endif
