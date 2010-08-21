#include "utilities.hpp"

#include <tuttle/common/exceptions.hpp>

#include <ofxCore.h>

#include <boost/lexical_cast.hpp>

namespace tuttle {
namespace ofx {

/// get me deepest bit depth
std::string findDeepestBitDepth( const std::string& s1, const std::string& s2 )
{
	if( s1 == kOfxBitDepthNone )
	{
		return s2;
	}
	else if( s1 == kOfxBitDepthByte )
	{
		if( s2 == kOfxBitDepthShort || s2 == kOfxBitDepthFloat )
			return s2;
		return s1;
	}
	else if( s1 == kOfxBitDepthShort )
	{
		if( s2 == kOfxBitDepthFloat )
			return s2;
		return s1;
	}
	else if( s1 == kOfxBitDepthFloat )
	{
		return s1;
	}
	else
	{
		BOOST_THROW_EXCEPTION( exception::Value()
			<< exception::user() + "Unrecognized bitdepth " + quotes(s1) + "."
			<< exception::dev() + "We can't find the deepest bit depth between " + quotes(s1) + " and " + quotes(s2) );
		return s2;
	}
}

}
}
