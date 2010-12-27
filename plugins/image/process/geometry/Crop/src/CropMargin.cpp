#include "CropMargin.hpp"
#include "CropPlugin.hpp"

#include <tuttle/plugin/opengl/gl.h>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace crop {

bool CropMargin::draw( const OFX::DrawArgs& args )
{
	CropPlugin* plugin = (CropPlugin*) _effect;

	if( plugin && plugin->displayRect() )
	{
		OfxRectD rect[2];
		rect[0] = plugin->getCropRect( args.time );
		// get the project size
		OfxPointD projSize, projOffset;
		projOffset = _effect->getProjectOffset();
		projSize   = _effect->getProjectSize();

		glPushMatrix();
		float x1 = float(projOffset.x + rect[0].x1);
		float y1 = float(projOffset.y + rect[0].y1);
		float x2 = float(projOffset.x + rect[0].x2);
		float y2 = float(projOffset.y + rect[0].y2);
		glColor3f( 1.0f, 1.00f, 0.0f );

		glBegin( GL_LINES );

		glVertex2f( x1, y1 );
		glVertex2f( x2, y1 );

		glVertex2f( x1, y2 );
		glVertex2f( x2, y2 );

		glVertex2f( x1, y1 );
		glVertex2f( x1, y2 );

		glVertex2f( x2, y1 );
		glVertex2f( x2, y2 );

		glEnd();
		glPopMatrix();
	}
	return true;
}

}
}
}
