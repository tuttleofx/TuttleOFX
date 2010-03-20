#ifndef _TUTTLE_PLUGIN_PARAMPOINTINTERACT_HPP_
#define	_TUTTLE_PLUGIN_PARAMPOINTINTERACT_HPP_

#include "PointInteract.hpp"

namespace tuttle {
namespace plugin {
namespace interact {

class ParamPointInteract : public PointInteract
{
public:
	typedef double Scalar;
	typedef boost::gil::point2<Scalar> Point2;
public:
	ParamPointInteract( const InteractInfos& infos, OFX::Double2DParam*, bool normalized = true );
	~ParamPointInteract();

private:
	OFX::Double2DParam* _param;

public:
	Point2 getPoint() const { return ofxToGil( _param->getValue() ); }
	void setPoint( const Scalar& x, const Scalar& y ) { _param->setValue( x, y ); }
};

}
}
}

#endif

