#include "CropMargin.hpp"
#include "CropPlugin.hpp"
#include "tuttle/plugin/interact/overlay.hpp"

#include <tuttle/plugin/opengl/gl.h>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace crop {

bool CropOverlay::draw( const OFX::DrawArgs& args )
{
	using namespace boost::gil;
	CropPlugin* plugin = (CropPlugin*) _effect;

	if( plugin && plugin->_paramOverlay->getValue() )
	{
		CropProcessParams<rgba32f_pixel_t> params = plugin->getProcessParams<rgba32f_pixel_t>( args.time, args.renderScale);
		glColor3f( 1.0f, 1.0f, 1.0f );
		overlay::displayRect( params._cropRegion );
		return true;
	}
	return false;
}

}
}
}
