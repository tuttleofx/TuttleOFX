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
	_gradationIn	= _plugin.getGradationLawIn();

	_GammaValueIn	= _plugin.getGammaValueIn();
	_BlackPointIn	= _plugin.getBlackPointValueIn();
	_WhitePointIn	= _plugin.getWhitePointValueIn();
	_GammaSensitoIn	= _plugin.getGammaSensitoValueIn();

	_layoutIn	= _plugin.getLayoutIn();
	_layoutOut	= _plugin.getLayoutOut();

	_tempColorIn	= _plugin.getTempColorIn();
	_tempColorOut	= _plugin.getTempColorOut();
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


	//const ttlc::GradationLaw::gamma _gradationGammaIn = { _plugin.getGammaValueIn() };


	#define CALL_PROCESS_FOR( eParamTempColorIn, structTempColorIn, structLayoutIn, structGradIn ) \
	case eParamTempColorIn : \
	{ \
		colorspace_pixels_progress< \
				ttlc::ttlc_colorspace< structGradIn, structLayoutIn >, \
				ttlc::ttlc_colorspace< ttlc::GradationLaw::linear, ttlc::Layout::RGB > \
		>( &csAPI, src, dst, this );\
	}

	// premacro
	#define LAYOUT_IN_FOR( eParamLayoutIn, structLayoutIn, structGradIn ) \
	case eParamLayoutIn : \
	{ \
		switch ( _tempColorIn ) \
		{ \
			CALL_PROCESS_FOR( ttlc::eParamTempA,		ColourTemp::A,		structLayoutIn, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempB,		ColourTemp::B,		structLayoutIn, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempC,		ColourTemp::C,		structLayoutIn, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempD50,		ColourTemp::D50,	structLayoutIn, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempD55,		ColourTemp::D55,	structLayoutIn, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempD58,		ColourTemp::D58,	structLayoutIn, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempD65,		ColourTemp::D65,	structLayoutIn, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempD75,		ColourTemp::D75,	structLayoutIn, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTemp9300,		ColourTemp::Temp9300,	structLayoutIn, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempF2,		ColourTemp::F2,		structLayoutIn, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempF7,		ColourTemp::F7,		structLayoutIn, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempF11,		ColourTemp::F11,	structLayoutIn, structGradIn )\
			CALL_PROCESS_FOR( ttlc::eParamTempDCIP3,	ColourTemp::DCIP3,	structLayoutIn, structGradIn )\
		} \
		break; \
	}

	#define GRADATION_IN_FOR( eParamGradationLawIn, structGradIn ) \
	case eParamGradationLawIn : \
	{ \
		switch ( _layoutIn ) \
		{ \
			LAYOUT_IN_FOR( ttlc::eParamLayoutRGB,		ttlc::Layout::RGB,	structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutYUV,		ttlc::Layout::YUV,	structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutYPbPr,		ttlc::Layout::YPbPr,	structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutHSV,		ttlc::Layout::HSV,	structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutHSL,		ttlc::Layout::HSL,	structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutLab,		ttlc::Layout::Lab,	structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutLuv,		ttlc::Layout::Luv,	structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutXYZ,		ttlc::Layout::XYZ,	structGradIn )\
			LAYOUT_IN_FOR( ttlc::eParamLayoutYxy,		ttlc::Layout::Yxy,	structGradIn )\
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
