
#ifndef _TUTTLE_COLORSPACE_API_HPP
#define _TUTTLE_COLORSPACE_API_HPP
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <map>
#include <vector>
#include <string>
#include <iostream>

namespace tuttle {
namespace plugin {
namespace color{

enum EParamReferenceSpace
{
	eParamSRGBREC709 = 0,
	eParamREC709,
	eParamREC601,
	eParamDCI,
	eParamACES,
	eParamProPhoto,
	eParamAdobe98
};

enum EParamGradationLaw
{
	eParamLinear = 0,
	eParamsRGB,
	eParamCineon,
	eParamGamma,
	eParamPanalog,
	eParamREDLog,
	eParamViperLog,
	eParamREDSpace,
	eParamAlexaLogC
};
const size_t numberOfGradationsModes = eParamAlexaLogC;

enum EParamLayout
{
	eParamLayoutRGB = 0,
	eParamLayoutYUV,
	eParamLayoutYPbPr,
	eParamLayoutHSV,
	eParamLayoutHSL,
	eParamLayoutLab,
	eParamLayoutLuv,
	eParamLayoutXYZ,
	eParamLayoutYxy
};

enum EParamTemp
{
	eParamTempA = 0,
	eParamTempB,
	eParamTempC,
	eParamTempD50,
	eParamTempD55,
	eParamTempD58,
	eParamTempD65,
	eParamTempD75,
	eParamTemp9300,
	eParamTempF2,
	eParamTempF7,
	eParamTempF11,
	eParamTempDCIP3
};

typedef std::map  < size_t, std::string > ColorSpaceMap;
typedef std::pair < size_t, std::string > ColorSpacePair;

namespace GradationLaw
{
	struct linear{};
	struct sRGB{};
	struct cineon{
		double blackPoint;
		double whitePoint;
		double gammaSensito;
	};
	struct gamma{
		double value;
	};
	struct panalog{};
	struct redLog{};
	struct viperLog{};
	struct redSpace{};
	struct alexaLogC{};
}

namespace Layout
{
	struct RGB{};
	struct YUV{};
	struct YPbPr{};
	struct HSV{};
	struct HSL{};
	struct Lab{};
	struct Luv{};
	struct XYZ{};
	struct Yxy{};
}

namespace ColourTemp
{
	struct A{};
	struct B{};
	struct C{};
	struct D50{};
	struct D55{};
	struct D58{};
	struct D65{};
	struct D75{};
	struct Temp9300{};
	struct F2{};
	struct F7{};
	struct F11{};
	struct DCIP3{};
}

namespace Primaries
{
	struct TODO{};
}

namespace Premultiplication
{
	struct On{};
	struct Off{};
}

/*
template < typename GradationLaw, typename Layout, typename Primaries, typename Premultiplication >
struct ttlc_colorspace {};
*/
template < typename GradationLaw, typename Layout >
struct ttlc_colorspace {
	GradationLaw	law;
	Layout		layout;
};


#include "GradationProcess.tcc"
#include "Layout.tcc"




class ColorSpaceAPI{
	ColorSpaceMap map_ReferenceSpace;
	ColorSpaceMap map_GradationLaw;
	ColorSpaceMap map_Layout;
	ColorSpaceMap map_ColourTemp;

	std::vector< std::vector< std::string > > colourSpaceGradationParameters;

public:
	ColorSpaceAPI();
	~ColorSpaceAPI();

	template < typename GradationLawIn, typename GradationLawOut, typename LayoutIn, typename LayoutOut, typename SrcP, typename DstP >
	bool colorspace_convert(
			const ttlc_colorspace< GradationLawIn,  LayoutIn  > inColorSpace,
			const ttlc_colorspace< GradationLawOut, LayoutOut > outColorSpace,
			const SrcP& src,
			      DstP& dst
			)
	{
		convertGradationLaw	( inColorSpace, src, dst );
		convertLayout		( inColorSpace, src, dst );
		return true;
	}

	size_t			sizeOfReferenceSpaces()						{ return map_ReferenceSpace.size(); }
	size_t			sizeOfGradationLaw()						{ return map_GradationLaw.size(); }
	size_t			sizeOfLayout()							{ return map_Layout.size(); }
	size_t			sizeOfColourTemp()						{ return map_ColourTemp.size(); }

	const ColorSpaceMap&	getMapReferenceSpaces()						{ return map_ReferenceSpace; }
	const ColorSpaceMap&	getMapGradationLaw()						{ return map_GradationLaw; }
	const ColorSpaceMap&	getMapLayout()							{ return map_Layout; }
	const ColorSpaceMap&	getMapColourTemp()						{ return map_ColourTemp; }

	size_t			sizeOfGradationParametersFor( EParamGradationLaw eGradLaw )	{ return colourSpaceGradationParameters.at( eGradLaw ).size(); }
};

}
}
}
#endif
