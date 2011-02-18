#ifndef _RECTOP_HPP
#define _RECTOP_HPP

#include "minmax.hpp"

#include <ofxCore.h>
#include <boost/gil/utilities.hpp>

#include <cmath>
#include <algorithm>

namespace tuttle {

template<class P>
inline OfxRectD pointsBoundingBox( const P& a, const P& b, const P& c, const P& d )
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
inline Point2 pointsMinXY( const std::vector<Point2>& points )
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
inline Point2 pointsMaxXY( const std::vector<Point2>& points )
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

template<class Point, class Rect>
inline bool pointInRect( const Point& p, const Rect& rec )
{
	Rect orientedRec;
	if( rec.x1 < rec.x2 )
	{
		orientedRec.x1 = rec.x1;
		orientedRec.x2 = rec.x2;
	}
	else
	{
		orientedRec.x1 = rec.x2;
		orientedRec.x2 = rec.x1;
	}
	if( rec.y1 < rec.y2 )
	{
		orientedRec.y1 = rec.y1;
		orientedRec.y2 = rec.y2;
	}
	else
	{
		orientedRec.y1 = rec.y2;
		orientedRec.y2 = rec.y1;
	}
	return p.x >= orientedRec.x1 && p.x <= orientedRec.x2 &&
	       p.y >= orientedRec.y1 && p.y <= orientedRec.y2;
}

template<class Rect>
inline Rect translateRegion( const Rect& windowRoW, const Rect& dependingTo )
{
	Rect windowOutput = windowRoW;
	windowOutput.x1 -= dependingTo.x1; // to output clip coordinates
	windowOutput.y1 -= dependingTo.y1;
	windowOutput.x2 -= dependingTo.x1;
	windowOutput.y2 -= dependingTo.y1;
	return windowOutput;
}

template<class Rect, class Point>
inline Rect translateRegion( const Rect& windowRoW, const Point& move )
{
	Rect windowOutput = windowRoW;
	windowOutput.x1 += move.x;
	windowOutput.y1 += move.y;
	windowOutput.x2 += move.x;
	windowOutput.y2 += move.y;
	return windowOutput;
}

template<class R>
inline R rectanglesBoundingBox( const R& a, const R& b )
{
	R res;
	res.x1 = std::min( a.x1, b.x1 );
	res.x2 = std::max( res.x1, std::max( a.x2, b.x2 ) );
	res.y1 = std::min( a.y1, b.y1 );
	res.y2 = std::max( res.y1, std::max( a.y2, b.y2 ) );
	return res;
}

template<class R>
inline R rectanglesIntersection( const R& a, const R& b )
{
	R res;
	res.x1 = std::max( a.x1, b.x1 );
	res.x2 = std::max( res.x1, std::min( a.x2, b.x2 ) );
	res.y1 = std::max( a.y1, b.y1 );
	res.y2 = std::max( res.y1, std::min( a.y2, b.y2 ) );
	return res;
}

template<class R, class V>
inline R rectangleGrow( const R& rect, const V marge )
{
	R res = rect;
	res.x1 -= marge;
	res.y1 -= marge;
	res.x2 += marge;
	res.y2 += marge;
	return res;
}

template<class R, class V>
inline R rectangleReduce( const R& rect, const V marge )
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

