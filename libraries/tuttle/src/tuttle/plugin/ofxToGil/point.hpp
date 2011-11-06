#ifndef _TUTTLE_OFXTOGIL_POINT_HPP_
#define	_TUTTLE_OFXTOGIL_POINT_HPP_

#include <ofxCore.h>

#include <boost/gil/utilities.hpp>

namespace tuttle {
namespace plugin {

inline boost::gil::point2<double> ofxToGil( const OfxPointD& p )
{
	return boost::gil::point2<double>( p.x, p.y );
}

inline OfxPointD gilToOfx( const boost::gil::point2<double>& p )
{
	OfxPointD r = { p.x, p.y };

	return r;
}

inline boost::gil::point2<int> ofxToGil( const OfxPointI& p )
{
	return boost::gil::point2<int>( p.x, p.y );
}

inline OfxPointI gilToOfx( const boost::gil::point2<int>& p )
{
	OfxPointI r = { p.x, p.y };

	return r;
}

}
}


#endif

