
#ifndef _TUTTLE_COLORSPACE_API_HPP
#define _TUTTLE_COLORSPACE_API_HPP
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <map>
#include <vector>
#include <string>
#include <iostream>

#include "colorDefinitions.hpp"

namespace tuttle {
namespace plugin {
namespace color{

/*
template < typename GradationLaw, typename Layout, typename Primaries, typename Premultiplication >
struct ttlc_colorspace {};
*/
template < typename GradationLaw, typename Layout >
struct ttlc_colorspace
{
	GradationLaw	law;
	Layout		layout;
};


#include "GradationProcess.tcc"
#include "Layout.tcc"


class ColorSpaceAPI
{
	GradationLaw::gamma	sGammaIn;
	GradationLaw::gamma	sGammaOut;
	GradationLaw::cineon	sCineonIn;
	GradationLaw::cineon	sCineonOut;

public:
	ColorSpaceAPI()
	{
		sGammaIn .value = 1.0;
		sGammaOut.value = 1.0;
		sCineonIn .blackPoint = 95.0;
		sCineonOut.blackPoint = 95.0;
		sCineonIn .whitePoint = 685.0;
		sCineonOut.whitePoint = 685.0;
		sCineonIn .gammaSensito = 300.0;
		sCineonOut.gammaSensito = 300.0;
	}
	~ColorSpaceAPI(){}

	void setGammaInProperties ( GradationLaw::gamma gamma )
	{
		sGammaIn = gamma;
	}
	void setGammaOutProperties ( GradationLaw::gamma gamma )
	{
		sGammaOut = gamma;
	}

	void setCineonInProperties ( GradationLaw::cineon cineon )
	{
		sCineonIn = cineon;
	}
	void setCineonOutProperties ( GradationLaw::cineon cineon )
	{
		sCineonOut = cineon;
	}

	template < typename SrcP, typename DstP >
	bool colorspace_convert(
			const EParamGradationLaw	eGradationLawIn,
			const EParamLayout		eLayoutIn,
			const EParamTemp		eTempIn,
			const EParamGradationLaw	eGradationLawOut,
			const EParamLayout		eLayoutOut,
			const EParamTemp		eTempOut,
			const SrcP&			src,
			      DstP&			dst
			)
	{
		DstP p0, p1, p2, p3, p4;
		switch ( eGradationLawIn )
		{
			case eParamLinear :	static_for_each( src, p0, computeFromLinear() );			break;
			case eParamsRGB :	static_for_each( src, p0, computeFromSRGB() );			break;
			case eParamCineon :	static_for_each( src, p0, computeFromCineon( sCineonIn.blackPoint, sCineonIn.whitePoint, sCineonIn.gammaSensito ) );	break;
			case eParamGamma :	static_for_each( src, p0, computeFromGamma( sGammaIn.value ) );	break;
			case eParamPanalog :	static_for_each( src, p0, computeFromPanalog() );			break;
			case eParamREDLog :	static_for_each( src, p0, computeFromRedLog() );			break;
			case eParamViperLog :	static_for_each( src, p0, computeFromViperLog() );			break;
			case eParamREDSpace :	static_for_each( src, p0, computeFromRedSpace() );			break;
			case eParamAlexaLogC :	static_for_each( src, p0, computeFromAlexaLogC() );			break;
		}
		switch ( eLayoutIn )
		{
			case eParamLayoutRGB :		convertFromRgbLayout	( p0, p1 ); break;
			case eParamLayoutYUV :		convertFromYuvLayout	( p0, p1 ); break;
			case eParamLayoutYPbPr :	convertFromYPbPrLayout	( p0, p1 ); break;
			case eParamLayoutHSV :		convertFromHsvLayout	( p0, p1 ); break;
			case eParamLayoutHSL :		convertFromHslLayout	( p0, p1 ); break;
			case eParamLayoutLab :		break;
			case eParamLayoutLuv :		break;
			case eParamLayoutXYZ :		break;
			case eParamLayoutYxy :		break;
		}
		switch ( eLayoutOut )
		{
			case eParamLayoutRGB :		convertToRgbLayout	( p1, p2 ); break;
			case eParamLayoutYUV :		convertToYuvLayout	( p1, p2 ); break;
			case eParamLayoutYPbPr :	convertToYPbPrLayout	( p1, p2 ); break;
			case eParamLayoutHSV :		convertToHsvLayout	( p1, p2 ); break;
			case eParamLayoutHSL :		convertToHslLayout	( p1, p2 ); break;
			case eParamLayoutLab :		break;
			case eParamLayoutLuv :		break;
			case eParamLayoutXYZ :		break;
			case eParamLayoutYxy :		break;
		}
		switch ( eGradationLawOut )
		{
			case eParamLinear :	static_for_each( p2, dst, computeToLinear() );			break;
			case eParamsRGB :	static_for_each( p2, dst, computeToSRGB() );			break;
			case eParamCineon :	static_for_each( p2, dst, computeToCineon( sCineonOut.blackPoint, sCineonOut.whitePoint, sCineonOut.gammaSensito ) );	break;
			case eParamGamma :	static_for_each( p2, dst, computeToGamma( sGammaOut.value ) );	break;
			case eParamPanalog :	static_for_each( p2, dst, computeToPanalog() );			break;
			case eParamREDLog :	static_for_each( p2, dst, computeToRedLog() );			break;
			case eParamViperLog :	static_for_each( p2, dst, computeToViperLog() );			break;
			case eParamREDSpace :	static_for_each( p2, dst, computeToRedSpace() );			break;
			case eParamAlexaLogC :	static_for_each( p2, dst, computeToAlexaLogC() );			break;
		}
		return true;
	}

};

}
}
}
#endif
