#ifndef _TUTTLE_PLUGIN_INTERACTOBJECT_HPP_
#define	_TUTTLE_PLUGIN_INTERACTOBJECT_HPP_

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
	typedef boost::gil::point2<double> Point2;
public:
	virtual ~InteractObject() = 0;

	/** @brief the function called to draw in the interact */
	virtual bool draw( const OFX::DrawArgs& args ) const { return false; }

	virtual EMoveType selectIfIntesect( const OFX::PenArgs& args ){ return eMoveTypeNone; }
	virtual bool selectIfIsIn( const OfxRectD& ){ return false; }
	virtual void unselect(){}

	virtual void endMove(){}
	virtual bool moveXYSelected( const Point2& ){ return false; }
	virtual bool moveXSelected( const Point2& ){ return false; }
	virtual bool moveYSelected( const Point2& ){ return false; }
	virtual void beginMove(){}
};

}
}
}

#endif
