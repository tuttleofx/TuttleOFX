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

	_GammaValueIn		= _plugin.getGammaValueIn();
	_BlackPointIn		= _plugin.getBlackPointValueIn();
	_WhitePointIn		= _plugin.getWhitePointValueIn();
	_GammaSensitoIn		= _plugin.getGammaSensitoValueIn();

	_gradationOut		= _plugin.getGradationLawOut();

	_GammaValueOut		= _plugin.getGammaValueOut();
	_BlackPointOut		= _plugin.getBlackPointValueOut();
	_WhitePointOut		= _plugin.getWhitePointValueOut();
	_GammaSensitoOut	= _plugin.getGammaSensitoValueOut();

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

/*
			TEMP_COLOR_IN_FOR( ttlc::eParamTempA,		ColourTemp::A,		structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			TEMP_COLOR_IN_FOR( ttlc::eParamTempB,		ColourTemp::B,		structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			TEMP_COLOR_IN_FOR( ttlc::eParamTempC,		ColourTemp::C,		structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			TEMP_COLOR_IN_FOR( ttlc::eParamTempD50,		ColourTemp::D50,	structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			TEMP_COLOR_IN_FOR( ttlc::eParamTempD55,		ColourTemp::D55,	structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			TEMP_COLOR_IN_FOR( ttlc::eParamTempD58,		ColourTemp::D58,	structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			TEMP_COLOR_IN_FOR( ttlc::eParamTempD65,		ColourTemp::D65,	structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			TEMP_COLOR_IN_FOR( ttlc::eParamTempD75,		ColourTemp::D75,	structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			TEMP_COLOR_IN_FOR( ttlc::eParamTemp9300,	ColourTemp::Temp9300,	structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			TEMP_COLOR_IN_FOR( ttlc::eParamTempF2,		ColourTemp::F2,		structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			TEMP_COLOR_IN_FOR( ttlc::eParamTempF7,		ColourTemp::F7,		structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			TEMP_COLOR_IN_FOR( ttlc::eParamTempF11,		ColourTemp::F11,	structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			TEMP_COLOR_IN_FOR( ttlc::eParamTempDCIP3,	ColourTemp::DCIP3,	structLayoutOut, structLayoutIn, structGradOut, structGradIn )\

		switch ( _layoutOut ) \
		{ \

		switch ( _layoutIn ) \
		{ \
			LAYOUT_IN_FOR( ttlc::eParamLayoutRGB,		ttlc::Layout::RGB,	structGradOut, structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutYUV,		ttlc::Layout::YUV,	structGradOut, structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutYPbPr,		ttlc::Layout::YPbPr,	structGradOut, structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutHSV,		ttlc::Layout::HSV,	structGradOut, structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutHSL,		ttlc::Layout::HSL,	structGradOut, structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutLab,		ttlc::Layout::Lab,	structGradOut, structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutLuv,		ttlc::Layout::Luv,	structGradOut, structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutXYZ,		ttlc::Layout::XYZ,	structGradOut, structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutYxy,		ttlc::Layout::Yxy,	structGradOut, structGradIn )\

*/

}

}
}
}
