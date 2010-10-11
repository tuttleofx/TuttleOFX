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
	Point2 _offset;

public:
	double         getTime() const { return _infos._effect->timeLineGetTime(); }
	virtual Point2 getPoint() const                           = 0;
	virtual void   setPoint( const Scalar x, const Scalar y ) = 0;
	virtual double getMarge() const { return _infos._marge * _infos._projectSize.x; }

	virtual bool      draw( const OFX::DrawArgs& args ) const;
	virtual EMoveType selectIfIntesect( const OFX::PenArgs& args );
	virtual bool      selectIfIsIn( const OfxRectD& );
	virtual bool      moveXYSelected( const Point2& );
	virtual bool      moveXSelected( const Point2& );
	virtual bool      moveYSelected( const Point2& );

};

}
}
}

#endif

