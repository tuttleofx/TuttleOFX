#ifndef _TUTTLE_PLUGIN_COORDONATESYSTEM_HPP_
#define	_TUTTLE_PLUGIN_COORDONATESYSTEM_HPP_


namespace tuttle {
namespace plugin {


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
//		COUT( "canonicalXY : " << point.x << ", " << point.y );
//		Point p( point.x / imgSize.x, ((point.y+((imgSize.x-imgSize.y)*0.5)) / imgSize.x) );
//		COUT( "normalizedXX : " << p.x << ", " << p.y );
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
	p.x = (point.x/imgSize.x);
	p.y = ((point.y+((imgSize.x-imgSize.y)*0.5)) / imgSize.x);
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
	p.y = (point.y * imgSize.x)-((imgSize.x-imgSize.y)*0.5);
	return p;
}

template<typename Point>
Point pointNormalizedXXcToCanonicalXY( const Point& point, const Point& imgSize )
{
	Point p;
	p.x = (point.x+0.5) * imgSize.x;
	p.y = ((point.y+0.5) * imgSize.x)-((imgSize.x-imgSize.y)*0.5);
	return p;
}

}
}


#endif


