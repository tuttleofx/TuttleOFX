#ifndef _TUTTLE_PLUGIN_INTERACTINFOS_HPP_
#define _TUTTLE_PLUGIN_INTERACTINFOS_HPP_

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>
#include <tuttle/plugin/coordinateSystem.hpp>

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
		: _effect( effect )
		, _focus( false )
		, _lastPenPos( 0, 0 )
		, _penDown( false )
		, _projectSize( ofxToGil( effect->getProjectSize() ) )
		, _marge( 0.01 )
	{}

	OFX::ImageEffect* _effect;
	bool _focus;
	Point2 _lastPenPos;
	bool _penDown;
	Point2 _projectSize; ///< ideally must be the RoW and not a project size...
	double _marge;

	template<typename Value>
	Value canonicalXXToNormalizedXX( const Value& point ) const { return canonicalXXToNormalizedXX( point, _projectSize ); }
	template<typename Value>
	Value normalizedXXToCanonicalXX( const Value& point ) const { return normalizedXXToCanonicalXX( point, _projectSize ); }

	template<typename Value>
	Value pointCanonicalXYToNormalizedXY( const Value& point ) const { return pointCanonicalXYToNormalizedXY( point, _projectSize ); }
	template<typename Point>
	Point pointNormalizedXYToCanonicalXY( const Point& point ) const { return pointNormalizedXYToCanonicalXY( point, _projectSize ); }

	//	template<typename Point>
	//    Point pointCanonicalXYToNormalizedXX( const Point& point ) const
	//	{
	//		return pointCanonicalXYToNormalizedXX( point, _imgSize );
	//	}
	template<typename Point>
	Point pointCanonicalXYToNormalizedXXc( const Point& point ) const
	{
		return pointCanonicalXYToNormalizedXXc( point, _projectSize );
	}

	//	template<typename Point>
	//    Point pointNormalizedXXToCanonicalXY( const Point& point ) const
	//	{
	//		return pointNormalizedXXToCanonicalXY( point, _imgSize );
	//	}
	template<typename Point>
	Point pointNormalizedXXcnToCanonicalXY( const Point& point ) const
	{
		return pointNormalizedXXcToCanonicalXY( point, _projectSize );
	}

};

}
}
}

#endif

