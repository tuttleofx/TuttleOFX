#ifndef _TUTTLE_PLUGIN_INTERACTOBJECT_HPP_
#define _TUTTLE_PLUGIN_INTERACTOBJECT_HPP_

#include "interact.hpp"
#include <ofxsInteract.h>
#include <boost/gil/utilities.hpp>

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
