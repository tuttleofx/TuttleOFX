#ifndef _TUTTLE_PLUGIN_PARAMPOINTINCLIPRELATIVEPOINT_HPP_
#define	_TUTTLE_PLUGIN_PARAMPOINTINCLIPRELATIVEPOINT_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "ParamPointInClip.hpp"
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

template<ECoordonateSystem coord>
class ParamPointInClipRelativePoint : public ParamPointInClip<coord>
{
public:
	ParamPointInClipRelativePoint( const InteractInfos& infos, OFX::Double2DParam* param, OFX::Clip* relativeClip, PointInteract* relativePoint );
	~ParamPointInClipRelativePoint();
	
private:
	PointInteract* _relativePoint;

public:
	Point2 getPoint() const;
	void setPoint( const Scalar& x, const Scalar& y );
};

template<ECoordonateSystem coord>
ParamPointInClipRelativePoint<coord>::ParamPointInClipRelativePoint( const InteractInfos& infos, OFX::Double2DParam* param, OFX::Clip* relativeClip, PointInteract* relativePoint )
: ParamPointInClip<coord>( infos, param, relativeClip )
, _relativePoint( relativePoint )
{
}

template<ECoordonateSystem coord>
ParamPointInClipRelativePoint<coord>::~ParamPointInClipRelativePoint( ) { }


template<>
Point2 ParamPointInClipRelativePoint<eCoordonateSystemXXcn>::getPoint() const
{
	Point2 point = ParamPointInClip<eCoordonateSystemXXcn>::getPoint();
	Point2 relativePoint = _relativePoint->getPoint();
	Point2 res = ParamPointInClip<eCoordonateSystemXXcn>::getPoint() + _relativePoint->getPoint();
	COUT( "ParamPointInClipRelativePoint<eCoordonateSystemXXcn>::getPoint()" );
	COUT_VAR( point );
	COUT_VAR( relativePoint );
	COUT_VAR( res );
	return res;
}

template<>
void ParamPointInClipRelativePoint<eCoordonateSystemXXcn>::setPoint( const Scalar& x, const Scalar& y )
{
	Point2 p = _relativePoint->getPoint();
	COUT( "ParamPointInClipRelativePoint<eCoordonateSystemXXcn>::setPoint()" );
	COUT_VAR2( x, y );
	COUT_VAR( p );
	ParamPointInClip<eCoordonateSystemXXcn>::setPoint( x - p.x, y - p.y );
}

}
}
}

#endif

