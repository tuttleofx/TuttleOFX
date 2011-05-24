
#include "colorSpaceAPI.hpp"
#include <cmath>

#include <boost/gil/pixel.hpp>

namespace tuttle {
namespace plugin {
namespace color{

ColorSpaceAPI::ColorSpaceAPI()
{
	map_ReferenceSpace.insert( std::pair< size_t, std::string >( eParamSRGBREC709,	"sRGB - Rec 709") );
	map_ReferenceSpace.insert( std::pair< size_t, std::string >( eParamREC709,	"Rec 709" ) );
	map_ReferenceSpace.insert( std::pair< size_t, std::string >( eParamREC601,	"Rec 601" ) );
	map_ReferenceSpace.insert( std::pair< size_t, std::string >( eParamDCI,		"DCI" ) );
	map_ReferenceSpace.insert( std::pair< size_t, std::string >( eParamACES,	"ACES" ) );
	map_ReferenceSpace.insert( std::pair< size_t, std::string >( eParamProPhoto,	"ProPhoto" ) );
	map_ReferenceSpace.insert( std::pair< size_t, std::string >( eParamAdobe98,	"Adobe 98" ) );


	map_GradationLaw.insert( std::pair< size_t, std::string >( eParamLinear,	"Linear" ) );
	map_GradationLaw.insert( std::pair< size_t, std::string >( eParamsRGB,		"sRGB" ) );
	map_GradationLaw.insert( std::pair< size_t, std::string >( eParamCineon,	"Cineon" ) );
	map_GradationLaw.insert( std::pair< size_t, std::string >( eParamGamma,		"Gamma" ) );
	map_GradationLaw.insert( std::pair< size_t, std::string >( eParamPanalog,	"Panalog" ) );
	map_GradationLaw.insert( std::pair< size_t, std::string >( eParamREDLog,	"REDLog" ) );
	map_GradationLaw.insert( std::pair< size_t, std::string >( eParamViperLog,	"ViperLog" ) );
	map_GradationLaw.insert( std::pair< size_t, std::string >( eParamREDSpace,	"REDSpace" ) );
	map_GradationLaw.insert( std::pair< size_t, std::string >( eParamAlexaLogC,	"AlexaLogC" ) );

	map_Layout.insert( std::pair< size_t, std::string >( eParamLayoutRGB,	"RGB" ) );
	map_Layout.insert( std::pair< size_t, std::string >( eParamLayoutYUV,	"YUV / Y Cr Cb" ) );
	map_Layout.insert( std::pair< size_t, std::string >( eParamLayoutYPbPr,	"Y Pr Pb" ) );
	map_Layout.insert( std::pair< size_t, std::string >( eParamLayoutHSV,	"HSV" ) );
	map_Layout.insert( std::pair< size_t, std::string >( eParamLayoutHSL,	"HSL" ) );
	map_Layout.insert( std::pair< size_t, std::string >( eParamLayoutLab,	"Lab" ) );
	map_Layout.insert( std::pair< size_t, std::string >( eParamLayoutLuv,	"Luv" ) );
	map_Layout.insert( std::pair< size_t, std::string >( eParamLayoutXYZ,	"XYZ" ) );
	map_Layout.insert( std::pair< size_t, std::string >( eParamLayoutYxy,	"Yxy" ) );

	map_ColourTemp.insert( std::pair< size_t, std::string >( eParamTempA,		"A - 2856°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eParamTempB,		"B - 4874°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eParamTempC,		"C - 6774°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eParamTempD50,		"D50 - 5003°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eParamTempD55,		"D55 - 5503°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eParamTempD58,		"D58 - 5803°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eParamTempD65,		"D65 - 6504°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eParamTempD75,		"D75 - 7504°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eParamTemp9300,	"9300°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eParamTempF2,		"F2 - 4230°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eParamTempF7,		"F7 - 6500°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eParamTempF11,		"F11 - 4000°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eParamTempDCIP3,	"DCI-P3 - ??°K" ) );

	for( size_t i = 0; i < numberOfGradationsModes; i++ )
	{
		std::vector< std::string > vec;
		colourSpaceGradationParameters.push_back( vec );
	}

	colourSpaceGradationParameters.at( eParamGamma  ) .push_back( "Gamma");
	colourSpaceGradationParameters.at( eParamCineon ) .push_back( "Black Point");
	colourSpaceGradationParameters.at( eParamCineon ) .push_back( "White Point");
	colourSpaceGradationParameters.at( eParamCineon ) .push_back( "Gamma Sensito");
}

ColorSpaceAPI::~ColorSpaceAPI()
{
}

/*
template < typename GradationLawIn, typename SrcP, typename DstP >
bool convert ( const ttlc_colorspace< GradationLawIn > inColorSpace, const SrcP& src, DstP& result )
{
	return false;
}*/
/*
template < typename SrcP, typename DstP >
bool convert( const ttlc_colorspace< GradationLaw::linear > inColorSpace, const SrcP& src, DstP& result )
{
	result = src;
	return true;
}


template < typename SrcP, typename DstP >
bool convert( const ttlc_colorspace< GradationLaw::sRGB     > inColorSpace, const SrcP& src, DstP& result )
{

	if( p > 0.04045 )
	{

		result = pow( (p+0.055)/1.055 , 2.4 );
		*p += sizeof(F);
		*result += sizeof(DstP);
		result = pow( (p+0.055)/1.055 , 2.4 );
	}
	else
		result = (double) p / 12.92;
	return true;
}

template < typename SrcP, typename DstP >
bool convert( const ttlc_colorspace< GradationLaw::cineon   > inColorSpace, const SrcP& src, DstP& result )
{
	return true;
}

template < typename SrcP, typename DstP >
bool convert( const ttlc_colorspace< GradationLaw::gamma    > inColorSpace, const SrcP& src, DstP& result )
{
	return true;
}

template < typename SrcP, typename DstP >
bool convert( const ttlc_colorspace< GradationLaw::panalog  > inColorSpace, const SrcP& src, DstP& result )
{
	return true;
}

template < typename SrcP, typename DstP >
bool convert( const ttlc_colorspace< GradationLaw::redLog   > inColorSpace, const SrcP& src, DstP& result )
{
	return true;
}

template < typename SrcP, typename DstP >
bool convert( const ttlc_colorspace< GradationLaw::viperLog > inColorSpace, const SrcP& src, DstP& result )
{
	return true;
}

template < typename SrcP, typename DstP >
bool convert( const ttlc_colorspace< GradationLaw::redSpace > inColorSpace, const SrcP& src, DstP& result )
{
	return true;
}

template < typename SrcP, typename DstP >
bool convert( const ttlc_colorspace< GradationLaw::alexaLogC> inColorSpace, const SrcP& src, DstP& result )
{
	return true;
}




#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>

template < typename GradationLawIn, typename GradationLawOut, typename ScrP, typename DstP >
bool colorspace_convert(
			const ttlc_colorspace< GradationLawIn > inColorSpace,
			const ttlc_colorspace< GradationLawOut > outColorSpace,
			const ScrP& src,
			DstP& dst
			)
{
	//std::cout << "convert input colorspace to lin / RGB / D65" << std::endl;
	convert( inColorSpace,  src, dst );
	//std::cout << "convert lin / RGB / D65 to output colorspace" << std::endl;
	return true;
}
template <typename SrcView, typename DstView>
convert( ttlc_colorspace< GradationLaw::linear >, pixel_type<SrcView>::type, pixel_type<DstView>::type );
*/


}
}
}
