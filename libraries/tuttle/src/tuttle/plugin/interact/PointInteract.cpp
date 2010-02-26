#include "PointInteract.hpp"

#include "interact.hpp"
#include "overlay.hpp"

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
	COUT_INFOS;
	
	typedef boost::gil::point2<double> Point2;
	double margeCanonical = _infos._margeDisplay * _infos._imgSize.x;
	
	glEnable( GL_LINE_STIPPLE );
//	glEnable( GL_LINE_STRIP );
	Point2 p( ofxToGil( _param->getValue() ) * _infos._imgSize );

	glLineStipple(1, 0xAAAA);
	overlay::displayPointRect( p, 0.5 * margeCanonical );
	glLineStipple(1, 0xFFFF);
	overlay::displayCross( p, 2.0 * margeCanonical );
	
	glDisable(GL_LINE_STIPPLE);
}


}
}
}
