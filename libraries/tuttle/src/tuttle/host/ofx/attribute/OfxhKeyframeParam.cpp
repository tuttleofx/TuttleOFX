#include "OfxhKeyframeParam.hpp"

#include <boost/throw_exception.hpp>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

void OfxhKeyframeParam::getNumKeys( unsigned int& nKeys ) const OFX_EXCEPTION_SPEC
{
	BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrMissingHostFeature ) );
}

void OfxhKeyframeParam::getKeyTime( int nth, OfxTime& time ) const OFX_EXCEPTION_SPEC
{
	BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrMissingHostFeature ) );
}

void OfxhKeyframeParam::getKeyIndex( const OfxTime time, int direction, const std::size_t index ) const OFX_EXCEPTION_SPEC
{
	BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrMissingHostFeature ) );
}

void OfxhKeyframeParam::deleteKey( const OfxTime time ) OFX_EXCEPTION_SPEC
{
	BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrMissingHostFeature ) );
}

void OfxhKeyframeParam::deleteAllKeys() OFX_EXCEPTION_SPEC
{
	BOOST_THROW_EXCEPTION( OfxhException( kOfxStatErrMissingHostFeature ) );
}

}
}
}
}

