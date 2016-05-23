
#ifndef _TUTTLE_COLOR_DEFINITIONS_HPP
#define _TUTTLE_COLOR_DEFINITIONS_HPP

#include <string>
#include <cstring>
#include <map>
#include <vector>

namespace tuttle
{
namespace plugin
{
namespace color
{

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

enum EColorTemperature
{
    eColorTemperatureA = 0,
    eColorTemperatureB,
    eColorTemperatureC,
    eColorTemperatureD50,
    eColorTemperatureD55,
    eColorTemperatureD58,
    eColorTemperatureD65,
    eColorTemperatureD75,
    eColorTemperature9300,
    eColorTemperatureE,
    eColorTemperatureF2,
    eColorTemperatureF7,
    eColorTemperatureF11,
    eColorTemperatureDCIP3
};

typedef std::map<size_t, std::string> ColorSpaceMap;
typedef std::pair<size_t, std::string> ColorSpacePair;

namespace GradationLaw
{
struct linear
{
};
struct sRGB
{
};
struct cineon
{
    double blackPoint;
    double whitePoint;
    double gammaSensito;
};
struct gamma
{
    double value;
};
struct panalog
{
};
struct redLog
{
};
struct viperLog
{
};
struct redSpace
{
};
struct alexaLogC
{
};
}

namespace Layout
{
struct RGB
{
};
struct YUV
{
};
struct YPbPr
{
};
struct HSV
{
};
struct HSL
{
};
struct Lab
{
};
struct Luv
{
};
struct XYZ
{
};
struct Yxy
{
};
}

namespace ColourTemp
{
struct A
{
};
struct B
{
};
struct C
{
};
struct D50
{
};
struct D55
{
};
struct D58
{
};
struct D65
{
};
struct D75
{
};
struct Temp9300
{
};
struct F2
{
};
struct F7
{
};
struct F11
{
};
struct DCIP3
{
};
}

namespace Primaries
{
struct TODO
{
};
}

namespace Premultiplication
{
struct On
{
};
struct Off
{
};
}

class ColorSpaceMaps
{
    ColorSpaceMap map_ReferenceSpace;
    ColorSpaceMap map_GradationLaw;
    ColorSpaceMap map_Layout;
    ColorSpaceMap map_ColourTemp;

    std::vector<std::vector<std::string> > colourSpaceGradationParameters;

public:
    ColorSpaceMaps();
    ~ColorSpaceMaps() {}

    const ColorSpaceMap& getMapReferenceSpaces() { return map_ReferenceSpace; }
    const ColorSpaceMap& getMapGradationLaw() { return map_GradationLaw; }
    const ColorSpaceMap& getMapLayout() { return map_Layout; }
    const ColorSpaceMap& getMapColourTemp() { return map_ColourTemp; }

    size_t sizeOfReferenceSpaces() { return map_ReferenceSpace.size(); }
    size_t sizeOfGradationLaw() { return map_GradationLaw.size(); }
    size_t sizeOfLayout() { return map_Layout.size(); }
    size_t sizeOfColourTemp() { return map_ColourTemp.size(); }

    size_t sizeOfGradationParametersFor(EParamGradationLaw eGradLaw)
    {
        return colourSpaceGradationParameters.at(eGradLaw).size();
    }
};
}
}
}
#endif
