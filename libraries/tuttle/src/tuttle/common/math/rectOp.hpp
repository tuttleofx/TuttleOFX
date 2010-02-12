#ifndef _RECTOP_HPP
#define _RECTOP_HPP
#include <cmath>
#include <algorithm>
#include "minmax.hpp"
#include "ofxCore.h"
#include <boost/gil/utilities.hpp>

namespace tuttle {

template<class P>
OfxRectD boundingBox( const P& a, const P& b, const P& c, const P& d )
{
	OfxRectD bb;

	bb.x1 = min( a.x, b.x, c.x, d.x );
	bb.y1 = min( a.y, b.y, c.y, d.y );
	bb.x2 = max( a.x, b.x, c.x, d.x );
	bb.y2 = max( a.y, b.y, c.y, d.y );
	return bb;
}

// template<typename Scalar>
inline OfxRectD boundingBox( const std::vector<bgil::point2<double> >& points )
{
    // if( !points.size() )
    //  throw...
    const bgil::point2<double> p( points[0] );
    OfxRectD bounds = { p.x, p.y, p.x, p.y};
    for( std::vector<bgil::point2<double> >::const_iterator it = points.begin(), itEnd = points.end();
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
R intersection( const R& a, const R& b )
{
	using std::min;
	using std::max;
	R bb;
	bb.x1 = max( a.x1, b.x1 );
	bb.x2 = max( bb.x1, min( a.x2, b.x2 ) );
	bb.y1 = max( a.y1, b.y1 );
	bb.y2 = max( bb.y1, min( a.y2, b.y2 ) );
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

#endif // _RECTOP_HPP

