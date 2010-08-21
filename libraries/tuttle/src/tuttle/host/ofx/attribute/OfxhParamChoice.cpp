#include "OfxhParamChoice.hpp"

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {


int OfxhParamChoice::getIndexFor( const std::string& key ) const
{
	typedef std::vector<std::string> StringVector;
	const StringVector& values = this->getProperties().fetchStringProperty(kOfxParamPropChoiceOption).getValues();
	StringVector::const_iterator itValue = std::find( values.begin(), values.end(), key );
	if( itValue == values.end() )
	{
	    std::string errorMsg( std::string("The key \"") + key + "\" doesn't exist for choice param \"" + this->getName() + "\".\n" );
	    errorMsg += "Correct values are : [";
	    for( StringVector::const_iterator it = values.begin(), itEnd = values.end();
		     it != itEnd;
		     ++it )
		{
	        errorMsg +=  *it + ", ";
		}
	    errorMsg += "]";
		BOOST_THROW_EXCEPTION( OfxhException( errorMsg ) );
	}
	return boost::numeric_cast<int>( std::distance( values.begin(), itValue ) );
}

const std::string& OfxhParamChoice::getValueForId( const int id ) const
{
	typedef std::vector<std::string> StringVector;
	const StringVector& values = this->getProperties().fetchStringProperty(kOfxParamPropChoiceOption).getValues();
	return values[id];
}

/**
 * implementation of var args function
 */
void OfxhParamChoice::getV( va_list arg ) const OFX_EXCEPTION_SPEC
{
	int* value = va_arg( arg, int* );

	return get( *value );
}

/**
 * implementation of var args function
 */
void OfxhParamChoice::getV( const OfxTime time, va_list arg ) const OFX_EXCEPTION_SPEC
{
	int* value = va_arg( arg, int* );

	return getAtTime( time, *value );
}

/**
 * implementation of var args function
 */
void OfxhParamChoice::setV( va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
{
	int value = va_arg( arg, int );

	return set( value, change );
}

/**
 * implementation of var args function
 */
void OfxhParamChoice::setV( const OfxTime time, va_list arg, const EChange change ) OFX_EXCEPTION_SPEC
{
	int value = va_arg( arg, int );

	return setAtTime( time, value, change );
}



}
}
}
}

