#include "HistogramKeyerOverlay.hpp"
#include "HistogramKeyerPlugin.hpp"

#include <tuttle/plugin/opengl/gl.h>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/overlay.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>

#include <vector>

namespace tuttle {
namespace plugin {
namespace histogramKeyer {

HistogramKeyerOverlay::HistogramKeyerOverlay( OfxInteractHandle handle, OFX::ImageEffect* effect )
	: OFX::OverlayInteract( handle )
	, _infos( effect )
{
	_plugin = static_cast<HistogramKeyerPlugin*>( _effect );
}

bool HistogramKeyerOverlay::draw( const OFX::DrawArgs& args )
{
	typedef boost::gil::point2<Scalar> Point2;
	static const float lineWidth = 2.0;
	bool displaySomething = false;

	glColor3f( 0.0f, 1.0f, 1.0f );
	overlay::displayRect( OfxRectD{0, 0, 200, 600} );
	glColor3f( 1.0f, 0.0f, 0.0f );
	overlay::displayRect( OfxRectD{0.01, 0.05, 0.5, 0.6} );

	if( /*_plugin->_paramOverlay->getValue() &&*/
		_plugin->_clipSrc->isConnected() )
	{
		displaySomething = true;

		OfxRectD srcRod = _plugin->_clipSrc->getCanonicalRod( args.time );
		glColor3f( 0.0f, 0.0f, 1.0f );
		overlay::displayRect( srcRod, -10 );
	}
	return displaySomething;
}

bool HistogramKeyerOverlay::penMotion( const OFX::PenArgs& args )
{
	return false;
}

bool HistogramKeyerOverlay::penDown( const OFX::PenArgs& args )
{
	return false;
}

bool HistogramKeyerOverlay::penUp( const OFX::PenArgs& args )
{
	return false;
}

}
}
}
