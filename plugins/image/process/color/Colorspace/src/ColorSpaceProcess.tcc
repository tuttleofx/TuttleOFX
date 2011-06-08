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


namespace ttlc = tuttle::plugin::color;

namespace tuttle {
namespace plugin {
namespace colorspace {

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
	_gradationIn		= _plugin.getGradationLawIn();

	if( _gradationIn == ttlc::eParamGamma )
	{
		ttlc::GradationLaw::gamma gammaIn;
		gammaIn.value = _plugin.getGammaValueIn();
		csAPI.setGammaInProperties( gammaIn );
	}

	if( _gradationIn == ttlc::eParamCineon )
	{
		ttlc::GradationLaw::cineon cineonIn;
		cineonIn.blackPoint   = _plugin.getBlackPointValueIn();
		cineonIn.whitePoint   = _plugin.getWhitePointValueIn();
		cineonIn.gammaSensito = _plugin.getGammaSensitoValueIn();
		csAPI.setCineonInProperties( cineonIn );
	}

	_gradationOut		= _plugin.getGradationLawOut();

	if( _gradationOut == ttlc::eParamGamma )
	{
		ttlc::GradationLaw::gamma gammaOut;
		gammaOut.value = _plugin.getGammaValueOut();
		csAPI.setGammaOutProperties( gammaOut );
	}

	if( _gradationOut == ttlc::eParamCineon )
	{
		ttlc::GradationLaw::cineon cineonOut;
		cineonOut.blackPoint   = _plugin.getBlackPointValueOut();
		cineonOut.whitePoint   = _plugin.getWhitePointValueOut();
		cineonOut.gammaSensito = _plugin.getGammaSensitoValueOut();
		csAPI.setCineonOutProperties( cineonOut );
	}

	_layoutIn		= _plugin.getLayoutIn();
	_layoutOut		= _plugin.getLayoutOut();

	_tempColorIn		= _plugin.getTempColorIn();
	_tempColorOut		= _plugin.getTempColorOut();
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

	View src = subimage_view(	this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
					procWindowSize.x,
					procWindowSize.y );
	View dst = subimage_view(	this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
					procWindowSize.x,
					procWindowSize.y );

	colorspace_pixels_progress( &csAPI, _gradationIn, _layoutIn, _tempColorIn, _gradationOut, _layoutOut, _tempColorOut, src, dst, this );

}

}
}
}
