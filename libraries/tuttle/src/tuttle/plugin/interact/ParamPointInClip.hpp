#ifndef _TUTTLE_PLUGIN_PARAMPOINTINCLIP_HPP_
#define	_TUTTLE_PLUGIN_PARAMPOINTINCLIP_HPP_

#include "InteractInfos.hpp"
#include "InteractObject.hpp"
#include "PointInteract.hpp"
#include "interact.hpp"
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
	
private:
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
	COUT( "ParamPointInClip<eCoordonateSystemXXcn>::getPoint()" );
	if( _relativeClip->isConnected() )
	{
		COUT( "getPoint - relativeClip isConnected" );
		COUT_VAR( _param->getValue() );
		Point2 p = ofxToGil( interact::pointNormalizedXXcToCanonicalXY( _param->getValue(), _relativeClip->getCanonicalRodSize( this->getTime() ) ) );
		COUT_VAR( p );
		return p;
		//p.x = ( p.x + 0.5 ) * width + rod.x1;
		//p.y = ( p.y + 0.5 ) * width + rod.y1 - (width - height) * 0.5;
	}
	COUT( "getPoint - relativeClip is not connected" );
	return Point2( 0, 0 );
}

template<>
void ParamPointInClip<eCoordonateSystemXXcn>::setPoint( const Scalar& x, const Scalar& y )
{
	COUT( "ParamPointInClip<eCoordonateSystemXXcn>::setPoint()" );
	COUT( "setPoint in canonical : " << "x:" << x << " y:" << y );
	if( _relativeClip->isConnected() )
	{
		OfxPointD p = { x, y };
		COUT( "setPoint - relativeClip isConnected" );
		p = interact::pointCanonicalXYToNormalizedXXc( p, _relativeClip->getCanonicalRodSize( this->getTime() ) );
		COUT( "setPoint centered normalized" << p );
//		Point2::value_type width  = rod.x2 - rod.x1;
//		Point2::value_type height = rod.y2 - rod.y1;
//		if( width )
//		{
//			p.x = ( ( p.x - rod.x1 ) / width ) - 0.5;
//			p.y = ( ( p.y - rod.y1 + (width - height) * 0.5 ) / width ) - 0.5;
//		}
		COUT_VAR(p);
		_param->setValue( p.x, p.y );
		return;
	}
	COUT( "setPoint - relativeClip is not connected" );
	_param->setValue( 0, 0 );
}

}
}
}

#endif

