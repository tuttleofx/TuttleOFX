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
	OfxRectD bb;

	bb.x1 = min( a.x, b.x, c.x, d.x );
	bb.y1 = min( a.y, b.y, c.y, d.y );
	bb.x2 = max( a.x, b.x, c.x, d.x );
	bb.y2 = max( a.y, b.y, c.y, d.y );
	return bb;
}

template<typename Point2>
inline OfxRectD pointsBoundingBox( const std::vector<Point2>& points )
{
    // if( !points.size() )
    //  throw...
    const Point2 p( points[0] );
    OfxRectD bounds = { p.x, p.y, p.x, p.y};
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

template<class R>
R rectanglesIntersection( const R& a, const R& b )
{
	R bb;
	bb.x1 = std::max( a.x1, b.x1 );
	bb.x2 = std::max( bb.x1, std::min( a.x2, b.x2 ) );
	bb.y1 = std::max( a.y1, b.y1 );
	bb.y2 = std::max( bb.y1, std::min( a.y2, b.y2 ) );
	/*    if ( bb.y2 < bb.y1 || bb.x2 < bb.x1 ) {
	 *      bb.x1 = 0;
	 *      bb.x2 = 0;
	 *      bb.y1 = 0;
	 *      bb.y2 = 0;
	 *  }
	 */
	return bb;
}

}

#endif

