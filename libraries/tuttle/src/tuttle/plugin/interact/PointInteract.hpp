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
	typedef boost::gil::point2<double> Point2;
public:
	PointInteract( const InteractInfos& infos, OFX::Double2DParam*, bool normalized = true );
	~PointInteract();
	
private:
	const InteractInfos& _infos;
	OFX::Double2DParam* _param;
	Point2 _offset;
	bool _normalized;

public:
	bool draw( const OFX::DrawArgs& args );
	EMoveType selectIfIntesect( const Point2& );
	bool selectIfIsIn( const OfxRectD& );
	bool moveXYSelected( const Point2& );
	bool moveXSelected( const Point2& );
	bool moveYSelected( const Point2& );

};

}
}
}

#endif

