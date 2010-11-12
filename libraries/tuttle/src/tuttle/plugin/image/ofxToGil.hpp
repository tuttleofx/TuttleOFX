#ifndef _TUTTLE_OFXTOGIL_HPP_
#define	_TUTTLE_OFXTOGIL_HPP_

#include <ofxCore.h>
#include <ofxPixels.h>

#include <boost/gil/utilities.hpp>
#include <boost/gil/rgba.hpp>

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


template<typename BitDepth>
inline typename boost::gil::pixel<BitDepth, boost::gil::rgba_layout_t> ofxToGil( const OfxRGBAColourD& c )
{
	return boost::gil::pixel<BitDepth, boost::gil::rgba_layout_t>( c.r, c.g, c.b, c.a );
}


#endif

