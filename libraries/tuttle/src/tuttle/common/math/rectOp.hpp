#ifndef _RECTOP_HPP
#define	_RECTOP_HPP

#include "minmax.hpp"

#include <ofxCore.h>
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

template<class R>
R intersection( const R& a, const R& b )
{
    R bb;
    bb.x1 = max( a.x1, b.x1 );
    bb.x2 = max(bb.x1, min( a.x2, b.x2 ));
    bb.y1 = max( a.y1, b.y1 );
    bb.y2 = max(bb.y1, min( a.y2, b.y2 ));
/*    if ( bb.y2 < bb.y1 || bb.x2 < bb.x1 ) {
        bb.x1 = 0;
        bb.x2 = 0;
        bb.y1 = 0;
        bb.y2 = 0;
    }
*/
    return bb;
}

}

#endif // _RECTOP_HPP

