#include "PointInteract.hpp"

#include "interact.hpp"
#include "overlay.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>
#include <tuttle/common/math/rectOp.hpp>

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace interact {

PointInteract::PointInteract( const InteractInfos& infos )
	: _infos( infos )
{}

PointInteract::~PointInteract() {}

bool PointInteract::draw( const OFX::DrawArgs& args ) const
{
	const double margeCanonical = getMarge() * args.pixelScale.x;
	const Point2 p( getPoint() );

	glEnable( GL_LINE_STIPPLE );
	glLineWidth( getSelected() ? 4.0 : 1.0 );

	glLineStipple( 1, 0xAAAA );
	overlay::displayPointRect( p, margeCanonical );
	glLineStipple( 1, 0xFFFF );
	overlay::displayCross( p, 3.0 * margeCanonical );

	glLineWidth( 1.0 );
	glDisable( GL_LINE_STIPPLE );

	return true;
}

MotionType PointInteract::intersect( const OFX::PenArgs& args )
{
	const Point2 mouse = ofxToGil( args.penPosition );
	const Point2 p = getPoint();

	const double margeCanonical = getMarge() * args.pixelScale.x;
	MotionType m;
	m._mode = eMotionTranslate;
	m._axis = clicPoint( p, mouse, margeCanonical );
	return m;
}

bool PointInteract::isIn( const OfxRectD& rect )
{
	return pointInRect( getPoint(), rect );
}

}
}
}
