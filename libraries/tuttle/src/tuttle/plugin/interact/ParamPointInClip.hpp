#ifndef _TUTTLE_PLUGIN_PARAMPOINTINCLIP_HPP_
#define	_TUTTLE_PLUGIN_PARAMPOINTINCLIP_HPP_

#include "Frame.hpp"
#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "PointInteract.hpp"
#include "interact.hpp"
#include <tuttle/plugin/coordinateSystem.hpp>

#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

template<class TFrame, ECoordonateSystem coord>
class ParamPointInClip : public PointInteract
{
public:
	ParamPointInClip( const InteractInfos& infos, OFX::Double2DParam* param, const TFrame& relativeFrame )
	: PointInteract( infos )
	, _param( *param )
	, _relativeFrame( relativeFrame )
	{
	}
	~ParamPointInClip(){}

protected:
	OFX::Double2DParam& _param;
	const TFrame _relativeFrame;

public:
	Point2 getPoint() const;
	void setPoint( const Scalar x, const Scalar y );

};

template<class TFrame>
class ParamPointInClip<TFrame, eCoordonateSystemXXcn> : public PointInteract
{
public:
	ParamPointInClip( const InteractInfos& infos, OFX::Double2DParam* param, const TFrame& relativeFrame )
	: PointInteract( infos )
	, _param( *param )
	, _relativeFrame( relativeFrame )
	{
	}
	~ParamPointInClip(){}

protected:
	OFX::Double2DParam& _param;
	const TFrame _relativeFrame;

public:
	Point2 getPoint() const
	{
		if( _relativeFrame.isEnabled() )
		{
			OfxRectD rod = _relativeFrame.getFrame( this->getTime() );
			OfxPointD rodSize = { rod.x2-rod.x1, rod.y2-rod.y1 };
			Point2 p = ofxToGil( pointNormalizedXXcToCanonicalXY( _param.getValue(), rodSize ) ) + Point2( rod.x1, rod.y1 );
			return p;
		}
		return Point2( 0, 0 );
	}
	void setPoint( const Scalar x, const Scalar y )
	{
		if( _relativeFrame.isEnabled() )
		{
			OfxRectD rod = _relativeFrame.getFrame( this->getTime() );
			Point2 rodSize( rod.x2-rod.x1, rod.y2-rod.y1 );
			Point2 p( x-rod.x1, y-rod.y1 );
			p = pointCanonicalXYToNormalizedXXc( p, rodSize );
			_param.setValue( p.x, p.y );
			return;
		}
		_param.setValue( 0, 0 );
	}

};

}
}
}

#endif

