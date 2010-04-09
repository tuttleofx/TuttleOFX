#ifndef _TUTTLE_PLUGIN_PARAMPOINTINTERACT_HPP_
#define	_TUTTLE_PLUGIN_PARAMPOINTINTERACT_HPP_

#include "PointInteract.hpp"

namespace tuttle {
namespace plugin {
namespace interact {

/**
 * @brief Create an interact/overlay for a given Double2DParam.
 */
template<ECoordonateSystem coord>
class ParamPointInteract : public PointInteract
{
public:
	ParamPointInteract( const InteractInfos& infos, OFX::Double2DParam* );
	~ParamPointInteract();

private:
	OFX::Double2DParam& _param;

public:
	Point2 getPoint() const;
	void setPoint( const Scalar& x, const Scalar& y );
};


template<ECoordonateSystem coord>
ParamPointInteract<coord>::ParamPointInteract( const InteractInfos& infos, OFX::Double2DParam* param )
: PointInteract( infos )
, _param( *param )
{

}

template<ECoordonateSystem coord>
ParamPointInteract<coord>::~ParamPointInteract()
{
}

template<>
Point2 ParamPointInteract<eCoordonateSystemXY>::getPoint() const { return ofxToGil( _param.getValue() ); }

template<>
void ParamPointInteract<eCoordonateSystemXY>::setPoint( const Scalar& x, const Scalar& y ) { _param.setValue( x, y ); }

template<>
Point2 ParamPointInteract<eCoordonateSystemXXcn>::getPoint() const { return ( ofxToGil( _param.getValue() ) + Point2(0.5,0.5) ) * this->_infos._projectSize; }

template<>
void ParamPointInteract<eCoordonateSystemXXcn>::setPoint( const Scalar& x, const Scalar& y ) { _param.setValue( (x / this->_infos._projectSize.x) - 0.5, (y / this->_infos._projectSize.y) - 0.5 ); }

}
}
}

#endif

