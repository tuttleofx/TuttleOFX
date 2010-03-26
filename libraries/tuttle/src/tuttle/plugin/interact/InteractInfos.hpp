#ifndef _TUTTLE_PLUGIN_INTERACTINFOS_HPP_
#define	_TUTTLE_PLUGIN_INTERACTINFOS_HPP_

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

#include <ofxsCore.h>
#include <ofxsParam.h>
#include <ofxsImageEffect.h>

#include <boost/gil/utilities.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

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
Point pointCanonicalXYToNormalizedXXc( const Point& point, const Point& imgSize )
{
	if( imgSize.x == 0 )
	{
		Point p;
		p.x = 0;
		p.y = 0;
		return p;
	}
	Point p;
	p.x = (point.x/imgSize.x)-0.5;
	p.y = ((point.y+((imgSize.x-imgSize.y)*0.5)) / imgSize.x) - 0.5;
	return p;
}

//template<typename Point>
//Point pointNormalizedXXToCanonicalXY( const Point& point, const Point& imgSize )
//{
//	Point p;
//	p.x = point.x * imgSize.x;
//	p.y = (point.y * imgSize.x)-((imgSize.x-imgSize.y)*0.5);
//	return p;
//}

template<typename Point>
Point pointNormalizedXXcToCanonicalXY( const Point& point, const Point& imgSize )
{
	Point p;
	p.x = (point.x+0.5) * imgSize.x;
	p.y = ((point.y+0.5) * imgSize.x)-((imgSize.x-imgSize.y)*0.5);
	return p;
}



class InteractInfos
{
	typedef boost::gil::point2<double> Point2;
public:
    InteractInfos( OFX::ImageEffect* effect )
    : _effect(effect)
	, _focus(false)
	, _lastPenPos(0,0)
    , _penDown(false)
	, _projectSize( ofxToGil(effect->getProjectSize()) )
    , _marge(0.01)
    {
    }
	
	OFX::ImageEffect* _effect;
    bool _focus;
    Point2 _lastPenPos;
    bool _penDown;
    Point2 _projectSize; ///< ideally must be the RoW and not a project size...
	double _marge;

	template<typename Value>
    Value canonicalXXToNormalizedXX( const Value& point ) const { return interact::canonicalXXToNormalizedXX( point, _projectSize ); }
	template<typename Value>
    Value normalizedXXToCanonicalXX( const Value& point ) const { return interact::normalizedXXToCanonicalXX( point, _projectSize ); }

	template<typename Value>
    Value pointCanonicalXYToNormalizedXY( const Value& point ) const { return interact::pointCanonicalXYToNormalizedXY( point, _projectSize ); }
	template<typename Point>
    Point pointNormalizedXYToCanonicalXY( const Point& point ) const { return interact::pointNormalizedXYToCanonicalXY( point, _projectSize ); }

//	template<typename Point>
//    Point pointCanonicalXYToNormalizedXX( const Point& point ) const
//	{
//		return interact::pointCanonicalXYToNormalizedXX( point, _imgSize );
//	}
	template<typename Point>
    Point pointCanonicalXYToNormalizedXXc( const Point& point ) const
	{
		return interact::pointCanonicalXYToNormalizedXXc( point, _projectSize );
	}
//	template<typename Point>
//    Point pointNormalizedXXToCanonicalXY( const Point& point ) const
//	{
//		return interact::pointNormalizedXXToCanonicalXY( point, _imgSize );
//	}
	template<typename Point>
    Point pointNormalizedXXcnToCanonicalXY( const Point& point ) const
	{
		return interact::pointNormalizedXXcToCanonicalXY( point, _projectSize );
	}
};

}
}
}

#endif

