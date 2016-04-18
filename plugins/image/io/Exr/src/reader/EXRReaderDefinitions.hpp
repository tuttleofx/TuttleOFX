#ifndef _EXRREADER_DEFINITIONS_HPP_
#define _EXRREADER_DEFINITIONS_HPP_

#include <EXRDefinitions.hpp>

#include <tuttle/plugin/global.hpp>
#include <tuttle/ioplugin/context/ReaderDefinition.hpp>

namespace tuttle
{
namespace plugin
{
namespace exr
{
namespace reader
{

static const std::string kParamOutputRedIs = "redIs";
static const std::string kParamOutputGreenIs = "greenIs";
static const std::string kParamOutputBlueIs = "blueIs";
static const std::string kParamOutputAlphaIs = "alphaIs";

static const std::string kParamOutputData = "outputData";

static const std::string kParamFileBitDepth = "fileBitDepth";
}
}
}
}

#endif
