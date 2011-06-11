#ifndef _TUTTLE_PLUGIN_INTERACTOBJECT_HPP_
#define _TUTTLE_PLUGIN_INTERACTOBJECT_HPP_

#include "interact.hpp"
#include <ofxsInteract.h>
#include <boost/gil/utilities.hpp>

#include <boost/math/special_functions/pow.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

typedef double Scalar;
typedef boost::gil::point2<Scalar> Point2;

/**
 * @brief A base class for all object to use in interact or overlay.
 * All sub classes needs to implement virtual functions from OFX::InteractI
 */
class InteractObject /*: public OFX::InteractI*/
{
public:
	InteractObject();
	virtual ~InteractObject() = 0;

	/** @brief the function called to draw in the interact */
	virtual bool draw( const OFX::DrawArgs& args ) const { return false; }
	
	virtual MotionType intersect( const OFX::PenArgs& args )
	{
		MotionType m;
		m._axis = eAxisNone;
		m._mode = eMotionNone;
		return m;
	}
	virtual bool isIn( const OfxRectD& ) { return false; }

	virtual Point2 getPosition() const { return Point2(0.0, 0.0); }
	virtual void setPosition( const Point2& ) {}
	virtual void setPositionX( const Scalar x ) {}
	virtual void setPositionY( const Scalar y ) {}

	virtual void translate( const Point2& previous, const Point2& vec )
	{
		setPosition( previous + vec );
	}
	virtual void rotate( const Point2& previous, const Point2& center, const Point2& from, const Point2& vec )
	{
		using namespace boost::math;
		// a^2 = b^2 + c^2 - 2bc * cos(alpha)
		// alpha = -arccos( (a^2 - b^2 - c^2) / 2bc )
//		const double a = std::sqrt( pow<2>(penPosition.x - _beginPenPosition.x) + pow<2>(penPosition.y - _beginPenPosition.y) );
//		const double b = std::sqrt( pow<2>(_beginPenPosition.x - _manipulator->getPosition().x) + pow<2>(_beginPenPosition.y - _manipulator->getPosition().y) );
//		const double c = std::sqrt( pow<2>(penPosition.x - _manipulator->getPosition().x) + pow<2>(penPosition.y - _manipulator->getPosition().y) );

		//rotate( center, -std::acos( (pow<2>(a) - pow<2>(b) - pow<2>(c)) / (2*abs(b)*abs(c)) ) );
	}
	virtual void scale( const Point2& previous, const Point2& center, const Point2& factor )
	{
	}
	
	virtual void beginMove( const Point2& penPosition ) {}
	virtual void endMove( const Point2& penPosition ) {}

	bool getSelected() const { return _selected; }
	void setSelected( const bool s ) { _selected = s; }

private:
	bool _selected;
};

}
}
}

#endif
