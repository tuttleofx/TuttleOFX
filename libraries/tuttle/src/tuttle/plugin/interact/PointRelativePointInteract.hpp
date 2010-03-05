#ifndef _TUTTLE_PLUGIN_POINTRELATIVEPOINTINTERACT_HPP_
#define	_TUTTLE_PLUGIN_POINTRELATIVEPOINTINTERACT_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

class PointRelativePointInteract : public InteractObject
{
public:
	typedef boost::gil::point2<double> Point2;
public:
	PointRelativePointInteract( const InteractInfos& infos, OFX::Double2DParam*, OFX::Double2DParam*, bool normalized = true );
	~PointRelativePointInteract();
	
private:
	const InteractInfos& _infos;
	OFX::Double2DParam* _param;
	OFX::Double2DParam* _relativeParam;
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

