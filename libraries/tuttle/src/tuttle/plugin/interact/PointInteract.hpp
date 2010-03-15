#ifndef _TUTTLE_PLUGIN_POINTINTERACT_HPP_
#define	_TUTTLE_PLUGIN_POINTINTERACT_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

class PointInteract : public InteractObject
{
public:
	typedef double Scalar;
	typedef boost::gil::point2<Scalar> Point2;
public:
	PointInteract( const InteractInfos& infos, const bool normalized = true );
	virtual ~PointInteract() = 0;
	
protected:
	const InteractInfos& _infos;
	Point2 _offset;
	bool _normalized;

public:
	virtual Point2 getPoint() const = 0;
	virtual void setPoint( const Scalar& x, const Scalar& y ) = 0;
	
	virtual bool draw( const OFX::DrawArgs& args ) const;
	virtual EMoveType selectIfIntesect( const Point2& );
	virtual bool selectIfIsIn( const OfxRectD& );
	virtual bool moveXYSelected( const Point2& );
	virtual bool moveXSelected( const Point2& );
	virtual bool moveYSelected( const Point2& );

};

}
}
}

#endif

