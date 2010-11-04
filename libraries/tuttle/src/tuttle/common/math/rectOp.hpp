#ifndef _RECTOP_HPP
#define _RECTOP_HPP

#include "minmax.hpp"

#include <ofxCore.h>
#include <boost/gil/utilities.hpp>

#include <cmath>
#include <algorithm>

namespace tuttle {

template<class P>
OfxRectD pointsBoundingBox( const P& a, const P& b, const P& c, const P& d )
{
	OfxRectD res;
	res.x1 = min( a.x, b.x, c.x, d.x );
	res.y1 = min( a.y, b.y, c.y, d.y );
	res.x2 = max( a.x, b.x, c.x, d.x );
	res.y2 = max( a.y, b.y, c.y, d.y );
	return res;
}

template<typename Point2>
inline OfxRectD pointsBoundingBox( const std::vector<Point2>& points )
{
	// if( !points.size() )
	//  BOOST_THROW_EXCEPTION...
	const Point2 p( points[0] );
	OfxRectD bounds = { p.x, p.y, p.x, p.y };

	for( typename std::vector<Point2>::const_iterator it = points.begin(), itEnd = points.end();
	     it != itEnd;
	     ++it )
	{
		if( it->x < bounds.x1 )
			bounds.x1 = it->x;
		else if( it->x > bounds.x2 )
			bounds.x2 = it->x;

		if( it->y < bounds.y1 )
			bounds.y1 = it->y;
		else if( it->y > bounds.y2 )
			bounds.y2 = it->y;
	}
	return bounds;
}

template<class Point2>
Point2 pointsMinXY( const std::vector<Point2>& points )
{
	// if( !points.size() )
	//  BOOST_THROW_EXCEPTION...
	Point2 p = points[0];

	for( typename std::vector<Point2>::const_iterator it = points.begin() + 1, itEnd = points.end();
	     it != itEnd;
	     ++it )
	{
		if( it->x < p.x )
			p.x = it->x;
		if( it->y < p.y )
			p.y = it->y;
	}
	return p;
}

template<class Point2>
Point2 pointsMaxXY( const std::vector<Point2>& points )
{
	// if( !points.size() )
	//  BOOST_THROW_EXCEPTION...
	Point2 p = points[0];

	for( typename std::vector<Point2>::const_iterator it = points.begin() + 1, itEnd = points.end();
	     it != itEnd;
	     ++it )
	{
		if( it->x > p.x )
			p.x = it->x;
		if( it->y > p.y )
			p.y = it->y;
	}
	return p;
}

template<class R>
R rectanglesBoundingBox( const R& a, const R& b )
{
	R res;
	res.x1 = std::min( a.x1, b.x1 );
	res.x2 = std::max( res.x1, std::max( a.x2, b.x2 ) );
	res.y1 = std::min( a.y1, b.y1 );
	res.y2 = std::max( res.y1, std::max( a.y2, b.y2 ) );
	return res;
}

template<class R>
R rectanglesIntersection( const R& a, const R& b )
{
	R res;
	res.x1 = std::max( a.x1, b.x1 );
	res.x2 = std::max( res.x1, std::min( a.x2, b.x2 ) );
	res.y1 = std::max( a.y1, b.y1 );
	res.y2 = std::max( res.y1, std::min( a.y2, b.y2 ) );
	return res;
}

template<class R, class V>
R rectangleGrow( const R& rect, const V marge )
{
	R res = rect;
	res.x1 -= marge;
	res.y1 -= marge;
	res.x2 += marge;
	res.y2 += marge;
}

template<class R, class V>
R rectangleReduce( const R& rect, const V marge )
{
	R res = rect;
	res.x1 += marge;
	res.y1 += marge;
	res.x2 -= marge;
	res.y2 -= marge;
	return res;
}

}

#endif

