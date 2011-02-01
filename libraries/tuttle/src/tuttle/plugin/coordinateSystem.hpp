#ifndef _TUTTLE_PLUGIN_COORDONATESYSTEM_HPP_
#define _TUTTLE_PLUGIN_COORDONATESYSTEM_HPP_

#include <boost/gil/utilities.hpp>
#include <boost/static_assert.hpp>

namespace tuttle {
namespace plugin {

typedef ::boost::gil::point2<double> Point2Double;

enum ECoordinateSystem
{
	eCoordinateSystemXYc,
	eCoordinateSystemXY,
	eCoordinateSystemXYcn,
	eCoordinateSystemXYn,
	eCoordinateSystemXXc,
	eCoordinateSystemXX,
	eCoordinateSystemXXcn,
	eCoordinateSystemXXn,
};

template<ECoordinateSystem coord>
struct NotCenteredCoodinate { static const ECoordinateSystem value = coord; };
template<>
struct NotCenteredCoodinate<eCoordinateSystemXYc> { static const ECoordinateSystem value = eCoordinateSystemXY; };
template<>
struct NotCenteredCoodinate<eCoordinateSystemXYcn> { static const ECoordinateSystem value = eCoordinateSystemXYn; };
template<>
struct NotCenteredCoodinate<eCoordinateSystemXXc> { static const ECoordinateSystem value = eCoordinateSystemXX; };
template<>
struct NotCenteredCoodinate<eCoordinateSystemXXcn> { static const ECoordinateSystem value = eCoordinateSystemXXn; };


template<ECoordinateSystem coord>
struct NotNormalizedCoodinate { static const ECoordinateSystem value = coord; };
template<>
struct NotNormalizedCoodinate<eCoordinateSystemXYcn> { static const ECoordinateSystem value = eCoordinateSystemXYc; };
template<>
struct NotNormalizedCoodinate<eCoordinateSystemXYn> { static const ECoordinateSystem value = eCoordinateSystemXY; };
template<>
struct NotNormalizedCoodinate<eCoordinateSystemXXc> { static const ECoordinateSystem value = eCoordinateSystemXXc; };
template<>
struct NotNormalizedCoodinate<eCoordinateSystemXXn> { static const ECoordinateSystem value = eCoordinateSystemXX; };


template<ECoordinateSystem coord>
struct CoordinateSystemAxisXY { static const ECoordinateSystem value = coord; };
template<>
struct CoordinateSystemAxisXY<eCoordinateSystemXXc> { static const ECoordinateSystem value = eCoordinateSystemXYc; };
template<>
struct CoordinateSystemAxisXY<eCoordinateSystemXX> { static const ECoordinateSystem value = eCoordinateSystemXY; };
template<>
struct CoordinateSystemAxisXY<eCoordinateSystemXXcn> { static const ECoordinateSystem value = eCoordinateSystemXYcn; };
template<>
struct CoordinateSystemAxisXY<eCoordinateSystemXXn> { static const ECoordinateSystem value = eCoordinateSystemXYn; };


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
Point pointCanonicalXXToNormalizedXX( const Point& point, const Point& imgSize )
{
	Point p(0,0);

	if( imgSize.x != 0 )
		p.x = point.x / imgSize.x;
	if( imgSize.y != 0 )
		p.y = point.y / imgSize.x;
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

template<ECoordinateSystem from, ECoordinateSystem to>
inline Point2Double pointConvertCoordinateSystem( const Point2Double& point, const Point2Double& imgSize ) { BOOST_STATIC_ASSERT( ( from == to ) ); return point; }

template<>
inline Point2Double pointConvertCoordinateSystem<eCoordinateSystemXY, eCoordinateSystemXYn>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointCanonicalXYToNormalizedXY( point, imgSize );
}

template<>
inline Point2Double pointConvertCoordinateSystem<eCoordinateSystemXYn, eCoordinateSystemXY>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointNormalizedXYToCanonicalXY( point, imgSize );
}

template<>
inline Point2Double pointConvertCoordinateSystem<eCoordinateSystemXY, eCoordinateSystemXXn>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointCanonicalXYToNormalizedXX( point, imgSize );
}

template<>
inline Point2Double pointConvertCoordinateSystem<eCoordinateSystemXY, eCoordinateSystemXXcn>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointCanonicalXYToNormalizedXXc( point, imgSize );
}

template<>
inline Point2Double pointConvertCoordinateSystem<eCoordinateSystemXXn, eCoordinateSystemXX>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointNormalizedXXToCanonicalXX( point, imgSize );
}

template<>
inline Point2Double pointConvertCoordinateSystem<eCoordinateSystemXXn, eCoordinateSystemXY>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointNormalizedXXToCanonicalXY( point, imgSize );
}

template<>
inline Point2Double pointConvertCoordinateSystem<eCoordinateSystemXXcn, eCoordinateSystemXY>( const Point2Double& point, const Point2Double& imgSize )
{
	return pointNormalizedXXcToCanonicalXY( point, imgSize );
}

}
}

#endif

