#ifndef _TUTTLE_PLUGIN_PARAMPOINTRELATIVEPOINTINTERACT_HPP_
#define	_TUTTLE_PLUGIN_PARAMPOINTRELATIVEPOINTINTERACT_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "PointInteract.hpp"
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

class ParamPointRelativePointInteract : public PointInteract
{
public:
	typedef double Scalar;
	typedef boost::gil::point2<double> Point2;
public:
	ParamPointRelativePointInteract( const InteractInfos& infos, OFX::Double2DParam*, OFX::Double2DParam*, bool normalized = true );
	~ParamPointRelativePointInteract();
	
private:
	OFX::Double2DParam* _param;
	OFX::Double2DParam* _relativeParam;

public:
	Point2 getPoint() const
	{
		Point2 p( ofxToGil( _relativeParam->getValue() ) + ofxToGil( _param->getValue() ) );
		return p;
	}
	void setPoint( const Scalar& x, const Scalar& y )
	{
		Point2 p( x, y );
		p -= ofxToGil( _relativeParam->getValue() );
		_param->setValue( p.x, p.y );
	}

};

}
}
}

#endif

