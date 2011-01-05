#ifndef _TUTTLE_PLUGIN_COORDONATESYSTEM_HPP_
#define _TUTTLE_PLUGIN_COORDONATESYSTEM_HPP_

#include <boost/gil/utilities.hpp>
#include <boost/static_assert.hpp>

namespace tuttle {
namespace plugin {

typedef ::boost::gil::point2<double> Point2Double;

enum ECoordonateSystem
{
	eCoordonateSystemXYc,
	eCoordonateSystemXY,
	eCoordonateSystemXYcn,
	eCoordonateSystemXYn,
	eCoordonateSystemXXc,
	eCoordonateSystemXX,
	eCoordonateSystemXXcn,
	eCoordonateSystemXXn,
};

template<typename Value, typename Point>
Value canonicalXXToNormalizedXX( const Value& v, const Point& imgSize ) { return v / imgSize.x; }
template<typename Value, typename Point>
Value normalizedXXToCanonicalXX( const Value& v, const Point& imgSize ) { return v * imgSize.x; }

template<typename Point>
Point pointCanonicalXYToNormalizedXY( const Point& point, const Point& imgSize ) { return point / imgSize; }
template<typename Point>
Point pointNormalizedXYToCanonicalXY( const Point& point, const Point& imgSize ) { return point * imgSize; }

//	template<typename Point>
//    Point pointCanonicalXYToNormalizedXX( const Point& point, const Point& imgSize )
//	{
//		TUTTLE_TCOUT( "canonicalXY : " << point.x << ", " << point.y );
//		Point p( point.x / imgSize.x, ((point.y+((imgSize.x-imgSize.y)*0.5)) / imgSize.x) );
//		TUTTLE_TCOUT( "normalizedXX : " << p.x << ", " << p.y );
//		return p;
//	}

template<typename Point>
Point pointCanonicalXYToNormalizedXX( const Point& point, const Point& imgSize )
{
	if( imgSize.x == 0 )
	{
		Point p;
		p.x = 0;
		p.y = 0;
		return p;
	}
	Point p;
	p.x = ( point.x / imgSize.x );
	p.y = ( ( point.y + ( ( imgSize.x - imgSize.y ) * 0.5 ) ) / imgSize.x );
	return p;
}

template<typename Point>
Point pointCanonicalXYToNormalizedXXc( const Point& point, const Point& imgSize )
{
	Point p( pointCanonicalXYToNormalizedXX( point, imgSize ) );

	p.x -= 0.5;
	p.y -= 0.5;
	return p;
}

template<typename Point>
Point pointNormalizedXXToCanonicalXX( const Point& point, const Point& imgSize )
{
	Point p;

	p.x = point.x * imgSize.x;
	p.y = point.y * imgSize.x;
	return p;
}

template<typename Point>
Point pointNormalizedXXToCanonicalXY( const Point& point, const Point& imgSize )
{
	Point p;

	p.x = point.x * imgSize.x;
	p.y = ( point.y * imgSize.x ) - ( ( imgSize.x - imgSize.y ) * 0.5 );
	return p;
}

template<typename Point>
Point pointNormalizedXXcToCanonicalXY( const Point& point, const Point& imgSize )
{
	Point p;

	p.x = ( point.x + 0.5 ) * imgSize.x;
	p.y = ( ( point.y + 0.5 ) * imgSize.x ) - ( ( imgSize.x - imgSize.y ) * 0.5 );
	return p;
}

template<ECoordonateSystem from, ECoordonateSystem to>
inline Point2Double pointConvertCoordonateSystem( const Point2Double& point, const Point2Double& imgSize ) { BOOST_STATIC_ASSERT( ( from == to ) ); return point; }

template<>
inline Point2Double pointConvertCoordonateSystem<eCoordonateSystemXY, eCoordonateSystemXYn>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointCanonicalXYToNormalizedXY( point, imgSize );
}

template<>
inline Point2Double pointConvertCoordonateSystem<eCoordonateSystemXYn, eCoordonateSystemXY>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointNormalizedXYToCanonicalXY( point, imgSize );
}

template<>
inline Point2Double pointConvertCoordonateSystem<eCoordonateSystemXY, eCoordonateSystemXXn>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointCanonicalXYToNormalizedXX( point, imgSize );
}

template<>
inline Point2Double pointConvertCoordonateSystem<eCoordonateSystemXY, eCoordonateSystemXXcn>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointCanonicalXYToNormalizedXXc( point, imgSize );
}

template<>
inline Point2Double pointConvertCoordonateSystem<eCoordonateSystemXXn, eCoordonateSystemXX>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointNormalizedXXToCanonicalXX( point, imgSize );
}

template<>
inline Point2Double pointConvertCoordonateSystem<eCoordonateSystemXXn, eCoordonateSystemXY>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointNormalizedXXToCanonicalXY( point, imgSize );
}

template<>
inline Point2Double pointConvertCoordonateSystem<eCoordonateSystemXXcn, eCoordonateSystemXY>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointNormalizedXXcToCanonicalXY( point, imgSize );
}

}
}

#endif

