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

class InteractInfos
{
	typedef boost::gil::point2<double> Point2;
public:
    InteractInfos( OFX::ImageEffect* effect )
    : _effect(effect)
	, _focus(false)
	, _lastPenPos(0,0)
    , _penDown(false)
	, _imgSize( ofxToGil(effect->getProjectSize()) )
    , _marge(0.02)
    {
    }
	
	OFX::ImageEffect* _effect;
    bool _focus;
    Point2 _lastPenPos;
    bool _penDown;
    Point2 _imgSize;
	double _marge;

	template<typename Value>
    Value canonicalXXToNormalizedXX( const Value& v ) const { return v / _imgSize.x; }
	template<typename Value>
    Value normalizedXXToCanonicalXX( const Value& v ) const { return v * _imgSize.x; }

	template<typename Value>
    Value pointCanonicalXYToNormalizedXY( const Value& v ) const { return v / _imgSize; }
	template<typename Point>
    Point pointNormalizedXYToCanonicalXY( const Point& point ) const { return point * _imgSize; }

//	template<typename Point>
//    Point pointCanonicalXYToNormalizedXX( const Point& point ) const
//	{
//		COUT( "canonicalXY : " << point.x << ", " << point.y );
//		Point p( point.x / _imgSize.x, ((point.y+((_imgSize.x-_imgSize.y)*0.5)) / _imgSize.x) );
//		COUT( "normalizedXX : " << p.x << ", " << p.y );
//		return p;
//	}
//	template<typename Point>
//    Point pointNormalizedXXToCanonicalXY( const Point& point ) const
//	{
//		return Point( point.x * _imgSize.x, (point.y * _imgSize.x)-((_imgSize.x-_imgSize.y)*0.5) );
//	}
	template<typename Point>
    Point pointCanonicalXYToNormalizedXXc( const Point& point ) const
	{
		Point p( (point.x/_imgSize.x)-0.5, ((point.y+((_imgSize.x-_imgSize.y)*0.5)) / _imgSize.x) - 0.5 );
		return p;
	}
	template<typename Point>
    Point pointNormalizedXXcToCanonicalXY( const Point& point ) const
	{
		return Point( (point.x+0.5) * _imgSize.x, ((point.y+0.5) * _imgSize.x)-((_imgSize.x-_imgSize.y)*0.5) );
	}
};

}
}
}

#endif

