#ifndef _TUTTLE_PLUGIN_PARAMPOINTINCLIPRELATIVEPOINT_HPP_
#define	_TUTTLE_PLUGIN_PARAMPOINTINCLIPRELATIVEPOINT_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "ParamPointInClip.hpp"
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

template<class TFrame, ECoordonateSystem coord>
class ParamPointInClipRelativePoint : public ParamPointInClip<TFrame, coord>
{
public:
	ParamPointInClipRelativePoint( const InteractInfos& infos, OFX::Double2DParam* param, const TFrame& relativeFrame, const PointInteract* relativePoint )
	: ParamPointInClip<TFrame, coord>( infos, param, relativeFrame )
	, _relativePoint( *relativePoint )
	{
	}
	~ParamPointInClipRelativePoint(){}

private:
	const PointInteract& _relativePoint;

public:
	Point2 getPoint() const;
	void setPoint( const Scalar x, const Scalar y );
};


template<class TFrame>
class ParamPointInClipRelativePoint<TFrame, eCoordonateSystemXXcn> : public ParamPointInClip<TFrame, eCoordonateSystemXXcn>
{
public:
	ParamPointInClipRelativePoint( const InteractInfos& infos, OFX::Double2DParam* param, const TFrame& relativeFrame, const PointInteract* relativePoint )
	: ParamPointInClip<TFrame, eCoordonateSystemXXcn>( infos, param, relativeFrame )
	, _relativePoint( *relativePoint )
	{
	}
	~ParamPointInClipRelativePoint(){}

private:
	const PointInteract& _relativePoint;

public:
	Point2 getPoint() const
	{
		OfxRectD rod = this->_relativeFrame.getFrame( this->getTime() );
		Point2 rodSize( rod.x2-rod.x1, rod.y2-rod.y1 );
		Point2 relativePoint = _relativePoint.getPoint();
		Point2 paramPoint = ofxToGil( this->_param.getValue() );
		Point2 point = pointNormalizedXXToCanonicalXX( paramPoint, rodSize );
		Point2 res = relativePoint + point;
		return res;
	}

	void setPoint( const Scalar x, const Scalar y )
	{
		if( this->_relativeFrame.isEnabled() )
		{
			OfxRectD rod = this->_relativeFrame.getFrame( this->getTime() );
			Point2 rodSize( rod.x2-rod.x1, rod.y2-rod.y1 );
			Point2 point = pointCanonicalXYToNormalizedXX( Point2(x,y), rodSize );
			Point2 relativePoint = pointCanonicalXYToNormalizedXX( _relativePoint.getPoint(), rodSize );
			this->_param.setValue( point.x - relativePoint.x, point.y - relativePoint.y );
			return;
		}
		this->_param.setValue( 0, 0 );
	}
};



}
}
}

#endif

