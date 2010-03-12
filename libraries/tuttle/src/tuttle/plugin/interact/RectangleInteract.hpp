#ifndef _TUTTLE_PLUGIN_RECTANGLEINTERACT_HPP_
#define	_TUTTLE_PLUGIN_RECTANGLEINTERACT_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

class RectangleInteract : public InteractObject
{
public:
	typedef boost::gil::point2<double> Point2;
public:
	RectangleInteract( const InteractInfos& infos, OFX::Double2DParam*, OFX::Double2DParam*, bool normalized = true );
	~RectangleInteract();
	
private:
	const InteractInfos& _infos;
	OFX::Double2DParam* _paramA;
	OFX::Double2DParam* _paramB;
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

