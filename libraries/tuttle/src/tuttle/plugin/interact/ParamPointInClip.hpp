#ifndef _TUTTLE_PLUGIN_PARAMPOINTINCLIP_HPP_
#define	_TUTTLE_PLUGIN_PARAMPOINTINCLIP_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "PointInteract.hpp"
#include "interact.hpp"
#include <tuttle/plugin/coordonateSystem.hpp>

#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

template<ECoordonateSystem coord>
class ParamPointInClip : public PointInteract
{
public:
	ParamPointInClip( const InteractInfos& infos, OFX::Double2DParam* param, OFX::Clip* relativeClip );
	~ParamPointInClip();

protected:
	OFX::Double2DParam* _param;
	OFX::Clip* _relativeClip;

public:
	Point2 getPoint() const;
	void setPoint( const Scalar& x, const Scalar& y );

};



template<ECoordonateSystem coord>
ParamPointInClip<coord>::ParamPointInClip( const InteractInfos& infos, OFX::Double2DParam* param, OFX::Clip* relativeClip )
: PointInteract( infos )
, _param( param )
, _relativeClip( relativeClip )
{
}

template<ECoordonateSystem coord>
ParamPointInClip<coord>::~ParamPointInClip( ) { }


template<>
Point2 ParamPointInClip<eCoordonateSystemXXcn>::getPoint() const
{
	if( _relativeClip->isConnected() )
	{
		Point2 p = ofxToGil( pointNormalizedXXcToCanonicalXY( _param->getValue(), _relativeClip->getCanonicalRod( this->getTime() ) ) );
		return p;
	}
	return Point2( 0, 0 );
}

template<>
void ParamPointInClip<eCoordonateSystemXXcn>::setPoint( const Scalar& x, const Scalar& y )
{
	if( _relativeClip->isConnected() )
	{
		OfxPointD p = { x, y };
		p = pointCanonicalXYToNormalizedXXc( p, _relativeClip->getCanonicalRod( this->getTime() ) );
		_param->setValue( p.x, p.y );
		return;
	}
	_param->setValue( 0, 0 );
}

}
}
}

#endif

