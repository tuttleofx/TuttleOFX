
#include "colorDefinitions.hpp"


namespace tuttle {
namespace plugin {
namespace color{


ColorSpaceMaps::ColorSpaceMaps()
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

	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperatureA,		"A - 2856°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperatureB,		"B - 4874°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperatureC,		"C - 6774°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperatureD50,		"D50 - 5003°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperatureD55,		"D55 - 5503°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperatureD58,		"D58 - 5803°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperatureD65,		"D65 - 6504°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperatureD75,		"D75 - 7504°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperature9300,	"9300°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperatureE,		"E - 5454°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperatureF2,		"F2 - 4230°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperatureF7,		"F7 - 6500°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperatureF11,		"F11 - 4000°K" ) );
	map_ColourTemp.insert( std::pair< size_t, std::string >( eColorTemperatureDCIP3,	"DCI-P3 - ??°K" ) );

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


}
}
}

