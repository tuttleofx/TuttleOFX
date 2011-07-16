#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/plugin/image/gil/color.hpp>

#include <boost/gil/gil_all.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {
namespace colorspace {

namespace ttlc = tuttle::plugin::color;
using namespace boost::gil;

template<class View>
ColorSpaceProcess<View>::ColorSpaceProcess( ColorSpacePlugin& instance )
	: ImageGilFilterProcessor<View>( instance )
	, _plugin( instance )
{
}

template<class View>
void ColorSpaceProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	
	_params = _plugin.getProcessParams();
}


/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void ColorSpaceProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
					procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
					procWindowSize.x, procWindowSize.y );

	ttlc::ColorSpaceAPI		csAPI;
	csAPI.setGammaInProperties	( _params._sGammaIn );
	csAPI.setCineonInProperties	( _params._sCineonIn );
	csAPI.setGammaOutProperties	( _params._sGammaOut );
	csAPI.setCineonOutProperties	( _params._sCineonOut );

	colorspace_pixels_progress( &csAPI, _params._gradationIn, _params._layoutIn, _params._tempColorIn, _params._gradationOut, _params._layoutOut, _params._tempColorOut, src, dst, this );

}

}
}
}
