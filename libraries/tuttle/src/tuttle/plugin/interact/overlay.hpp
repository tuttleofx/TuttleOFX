#ifndef _TUTTLE_PLUGIN_OVERLAY_HPP_
#define _TUTTLE_PLUGIN_OVERLAY_HPP_

#include <ofxsInteract.h>
#include <tuttle/plugin/opengl/gl.h>

namespace tuttle {
namespace plugin {
namespace overlay {

template<typename Point2>
inline void displayCross( const Point2& p, const double marge )
{
	glBegin( GL_LINES );
	glVertex2f( p.x - marge, p.y );
	glVertex2f( p.x + marge, p.y );
	glVertex2f( p.x, p.y - marge );
	glVertex2f( p.x, p.y + marge );
	glEnd();
}

template<typename Point2>
inline void displayPointRect( const Point2& p, const Point2& size )
{
	glBegin( GL_LINE_LOOP );
	glVertex2f( p.x - size.x, p.y - size.y );
	glVertex2f( p.x + size.x, p.y - size.y );
	glVertex2f( p.x + size.x, p.y + size.y );
	glVertex2f( p.x - size.x, p.y + size.y );
	glEnd();
}

template<typename Point2>
inline void displayPointRect( const Point2& p, const double marge )
{
	glBegin( GL_LINE_LOOP );
	glVertex2f( p.x - marge, p.y - marge );
	glVertex2f( p.x + marge, p.y - marge );
	glVertex2f( p.x + marge, p.y + marge );
	glVertex2f( p.x - marge, p.y + marge );
	glEnd();
}

template<typename Rect>
inline void displayRect( const Rect& r, const double marge = 0 )
{
	glBegin( GL_LINE_LOOP );
	glVertex2f( r.x1 - marge, r.y1 - marge );
	glVertex2f( r.x2 + marge, r.y1 - marge );
	glVertex2f( r.x2 + marge, r.y2 + marge );
	glVertex2f( r.x1 - marge, r.y2 + marge );
	glEnd();
}

template<typename Point2>
inline void displayRect( const Point2& a, const Point2& b, const double marge = 0 )
{
	glBegin( GL_LINE_STRIP );
	glVertex2f( a.x - marge, a.y - marge );
	glVertex2f( b.x + marge, a.y - marge );
	glVertex2f( b.x + marge, b.y + marge );
	glVertex2f( a.x - marge, b.y + marge );
	glVertex2f( a.x - marge, a.y - marge );
	glEnd();
}

template<typename Point2>
inline void drawCurve( const std::vector<Point2>& vec )
{
	glBegin( GL_LINE_STRIP );
	for( typename std::vector<Point2>::const_iterator it = vec.begin(), itEnd = vec.end();
	     it != itEnd;
	     ++it )
	{
		glVertex2f( it->x, it->y );
	}
	glEnd();
}

template<typename Point2>
inline void drawCurves( const std::vector<std::vector<Point2> >& vec )
{
	for( typename std::vector<std::vector<Point2> >::const_iterator it = vec.begin(), itEnd = vec.end();
	     it != itEnd;
	     ++it )
	{
		drawCurve( *it );
	}
}

}
}
}

#endif

