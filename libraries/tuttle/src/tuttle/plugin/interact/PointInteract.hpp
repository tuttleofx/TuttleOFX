#ifndef _TUTTLE_PLUGIN_POINTINTERACT_HPP_
#define _TUTTLE_PLUGIN_POINTINTERACT_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

class PointInteract : public InteractObject
{
public:
	PointInteract( const InteractInfos& infos );
	virtual ~PointInteract() = 0;

protected:
	const InteractInfos& _infos;

public:
	double         getTime() const { return _infos._effect->timeLineGetTime(); }
	virtual Point2 getPoint() const                           = 0;
	virtual void   setPoint( const Scalar x, const Scalar y ) = 0;
	
	virtual Point2 getPosition() const { return getPoint(); }
	virtual void   setPosition( const Point2& p ) { setPoint(p.x, p.y); }
	virtual void   setPositionX( const Scalar x ) { setPoint( x, getPoint().y ); }
	virtual void   setPositionY( const Scalar y ) { setPoint( getPoint().x, y ); }

	virtual double getMarge() const { return _infos._marge * _infos._projectSize.x; }

	virtual bool      draw( const OFX::DrawArgs& args ) const;

	virtual MotionType intersect( const OFX::PenArgs& args );
	virtual bool      isIn( const OfxRectD& );
};

}
}
}

#endif

