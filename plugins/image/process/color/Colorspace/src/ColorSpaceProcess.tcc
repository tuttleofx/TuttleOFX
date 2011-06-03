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


	#define CALL_PROCESS_FOR( eParamTempColorOut, structTempColorOut, structTempColorIn, structLayoutOut, structLayoutIn, structGradOut, structGradIn ) \
	case eParamTempColorOut : \
	{ \
	}
		/*
		colorspace_pixels_progress< \
				ttlc::ttlc_colorspace< structGradIn, structLayoutIn >, \
				ttlc::ttlc_colorspace< ttlc::GradationLaw::linear, ttlc::Layout::RGB > \
		>( &csAPI, src, dst, this );\
		*/

	#define TEMP_COLOR_IN_FOR( eParamTempColorIn, structTempColorIn, structLayoutOut, structLayoutIn, structGradOut, structGradIn ) \
	case eParamTempColorIn : \
	{ \
		switch ( _tempColorOut ) \
		{ \
			CALL_PROCESS_FOR( ttlc::eParamTempA,		ColourTemp::A,		structTempColorOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempB,		ColourTemp::B,		structTempColorOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempC,		ColourTemp::C,		structTempColorOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempD50,		ColourTemp::D50,	structTempColorOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempD55,		ColourTemp::D55,	structTempColorOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempD58,		ColourTemp::D58,	structTempColorOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempD65,		ColourTemp::D65,	structTempColorOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempD75,		ColourTemp::D75,	structTempColorOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTemp9300,		ColourTemp::Temp9300,	structTempColorOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempF2,		ColourTemp::F2,		structTempColorOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempF7,		ColourTemp::F7,		structTempColorOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempF11,		ColourTemp::F11,	structTempColorOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempDCIP3,	ColourTemp::DCIP3,	structTempColorOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn )\
		} \
		break; \
	}

	#define LAYOUT_OUT_FOR( eParamLayoutOut, structLayoutOut, structLayoutIn, structGradOut, structGradIn ) \
	case eParamLayoutOut : \
	{ \
		switch ( _tempColorIn ) \
		{ \
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
		} \
		break; \
	}

	#define LAYOUT_IN_FOR( eParamLayoutIn, structLayoutIn, structGradOut, structGradIn ) \
	case eParamLayoutIn : \
	{ \
		switch ( _layoutOut ) \
		{ \
			LAYOUT_OUT_FOR( ttlc::eParamLayoutRGB,		ttlc::Layout::RGB,	structLayoutIn, structGradOut, structGradIn )\
			LAYOUT_OUT_FOR( ttlc::eParamLayoutYUV,		ttlc::Layout::YUV,	structLayoutIn, structGradOut, structGradIn )\
			LAYOUT_OUT_FOR( ttlc::eParamLayoutYPbPr,	ttlc::Layout::YPbPr,	structLayoutIn, structGradOut, structGradIn )\
			LAYOUT_OUT_FOR( ttlc::eParamLayoutHSV,		ttlc::Layout::HSV,	structLayoutIn, structGradOut, structGradIn )\
			LAYOUT_OUT_FOR( ttlc::eParamLayoutHSL,		ttlc::Layout::HSL,	structLayoutIn, structGradOut, structGradIn )\
			LAYOUT_OUT_FOR( ttlc::eParamLayoutLab,		ttlc::Layout::Lab,	structLayoutIn, structGradOut, structGradIn )\
			LAYOUT_OUT_FOR( ttlc::eParamLayoutLuv,		ttlc::Layout::Luv,	structLayoutIn, structGradOut, structGradIn )\
			LAYOUT_OUT_FOR( ttlc::eParamLayoutXYZ,		ttlc::Layout::XYZ,	structLayoutIn, structGradOut, structGradIn )\
			LAYOUT_OUT_FOR( ttlc::eParamLayoutYxy,		ttlc::Layout::Yxy,	structLayoutIn, structGradOut, structGradIn )\
		} \
		break; \
	}

	#define GRADATION_OUT_FOR( eParamGradationLawOut, structGradOut, structGradIn ) \
	case eParamGradationLawOut : \
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
		} \
		break; \
	}

	#define GRADATION_IN_FOR( eParamGradationLawIn, structGradIn ) \
	case eParamGradationLawIn : \
	{ \
		switch ( _gradationOut ) \
		{ \
			GRADATION_OUT_FOR( ttlc::eParamLinear,		ttlc::GradationLaw::linear	,structGradIn )\
			GRADATION_OUT_FOR( ttlc::eParamsRGB,		ttlc::GradationLaw::sRGB	,structGradIn )\
			GRADATION_OUT_FOR( ttlc::eParamCineon,		ttlc::GradationLaw::cineon	,structGradIn )\
			GRADATION_OUT_FOR( ttlc::eParamGamma,		ttlc::GradationLaw::gamma	,structGradIn )\
			GRADATION_OUT_FOR( ttlc::eParamPanalog,		ttlc::GradationLaw::panalog	,structGradIn )\
			GRADATION_OUT_FOR( ttlc::eParamREDLog,		ttlc::GradationLaw::redLog	,structGradIn )\
			GRADATION_OUT_FOR( ttlc::eParamViperLog,	ttlc::GradationLaw::viperLog	,structGradIn )\
			GRADATION_OUT_FOR( ttlc::eParamREDSpace,	ttlc::GradationLaw::redSpace	,structGradIn )\
			GRADATION_OUT_FOR( ttlc::eParamAlexaLogC,	ttlc::GradationLaw::alexaLogC	,structGradIn )\
		} \
		break; \
	}

	switch( _gradationIn )
	{
		GRADATION_IN_FOR( ttlc::eParamLinear,		ttlc::GradationLaw::linear	)
		GRADATION_IN_FOR( ttlc::eParamsRGB,		ttlc::GradationLaw::sRGB	)
		GRADATION_IN_FOR( ttlc::eParamCineon,		ttlc::GradationLaw::cineon	)
		GRADATION_IN_FOR( ttlc::eParamGamma,		ttlc::GradationLaw::gamma	)
		GRADATION_IN_FOR( ttlc::eParamPanalog,		ttlc::GradationLaw::panalog	)
		GRADATION_IN_FOR( ttlc::eParamREDLog,		ttlc::GradationLaw::redLog	)
		GRADATION_IN_FOR( ttlc::eParamViperLog,		ttlc::GradationLaw::viperLog	)
		GRADATION_IN_FOR( ttlc::eParamREDSpace,		ttlc::GradationLaw::redSpace	)
		GRADATION_IN_FOR( ttlc::eParamAlexaLogC,	ttlc::GradationLaw::alexaLogC	)
	}

	#undef GRADATION_IN_FOR
	#undef LAYOUT_IN_FOR
	#undef CALL_PROCESS_FOR


}

}
}
}
