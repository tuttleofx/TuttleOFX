#ifndef _TUTTLE_PLUGIN_PRINT_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_PRINT_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace print
{

static const std::string kParamMode = "mode";
static const std::string kParamModeImage = "image";
static const std::string kParamModeRegion = "region";
static const std::string kParamModePixel = "pixel";
enum EParamMode
{
    eParamModeImage = 0,
    eParamModeRegion,
    eParamModePixel
};

static const std::string kParamPixel = "pixel";
static const std::string kParamRegionMin = "regionMin";
static const std::string kParamRegionMax = "regionMax";

const static std::string kParamColumns = "columns";

const static std::string kParamColor = "color";
const static std::string kParamColorMono = "mono";
const static std::string kParamColorGray = "gray";
const static std::string kParamColor8 = "8ansi";
const static std::string kParamColor16 = "16ansi";
const static std::string kParamColorFullGray = "fullgray";
const static std::string kParamColorFull8 = "full8ansi";
const static std::string kParamColorFull16 = "full16ansi";

enum EParamColor
{
    eParamColorMono = 0,
    eParamColorGray,
    eParamColor8,
    eParamColor16,
    eParamColorfullgray,
    eParamColorfull8,
    eParamColorfull16,
};

const static std::string kParamOutput = "output";
const static std::string kParamOutputNumeric = "numeric";
const static std::string kParamOutputAscii = "ascii";

enum EParamOutput
{
    eParamOutputAscii = 0,
    eParamOutputNumeric
};
static const std::string kParamFlip = "flip";

const static std::string kParamOutputOpenGL = "opengl";
}
}
}

#endif
