#include "PointInteract.hpp"

#include "interact.hpp"
#include "overlay.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

PointInteract::PointInteract( const InteractInfos& infos, OFX::Double2DParam* param, bool normalized )
: _infos(infos)
, _param(param)
, _normalized(normalized)
{
}

PointInteract::~PointInteract( ) { }

bool PointInteract::draw( const OFX::DrawArgs& args )
{
	typedef boost::gil::point2<double> Point2;
	double margeCanonical = _infos.normalizedXXToCanonicalXX( _infos._marge );
	
	Point2 p( _infos.pointNormalizedXXcToCanonicalXY( ofxToGil( _param->getValue() ) ) );

//	COUT( "PointInteract::draw" );
//	COUT_VAR( _infos._imgSize );
//	COUT_VAR( p );

	glEnable( GL_LINE_STIPPLE );
	glColor3d(1.0,1.0,1.0);
	glLineStipple(1, 0xAAAA);
	overlay::displayPointRect( p, margeCanonical );
	glLineStipple(1, 0xFFFF);
	overlay::displayCross( p, 2.0 * margeCanonical );
	
	glDisable(GL_LINE_STIPPLE);
}

EMoveType PointInteract::selectIfIntesect( const Point2& point )
{
	Point2 p( ofxToGil( _param->getValue() ) );
	EMoveType m = clicPoint( p, point, _infos._marge );
	COUT("selectIfIntesect");
	COUT_VAR(point);
	COUT_VAR(p);
	switch(m)
	{
		case eMoveTypeXY:
			COUT("eMoveTypeXY");
			break;
		case eMoveTypeX:
			COUT("eMoveTypeX");
			break;
		case eMoveTypeY:
			COUT("eMoveTypeY");
			break;
		case eMoveTypeNone:
			COUT("eMoveTypeNone");
			break;
	}
	return m;
}

bool PointInteract::selectIfIsIn( const OfxRectD& )
{
	return false;
}

bool PointInteract::moveXYSelected( const Point2& point )
{
	_param->setValue( point.x, point.y );
	return true;
}

bool PointInteract::moveXSelected( const Point2& point )
{
	_param->setValue( point.x, _param->getValue().y );
	return true;
}

bool PointInteract::moveYSelected( const Point2& point )
{
	_param->setValue( _param->getValue().x, point.y );
	return true;
}

}
}
}
