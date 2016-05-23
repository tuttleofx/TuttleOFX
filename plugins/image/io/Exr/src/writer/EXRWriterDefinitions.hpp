#ifndef EXRWRITER_DEFINITIONS_HPP
#define EXRWRITER_DEFINITIONS_HPP

#include <EXRDefinitions.hpp>

#include <tuttle/plugin/global.hpp>
#include <tuttle/ioplugin/context/WriterDefinition.hpp>

namespace tuttle
{
namespace plugin
{
namespace exr
{
namespace writer
{

static const std::string kParamStorageType = "storage";
static const std::string kParamStorageScanLine = "scanLine";
static const std::string kParamStorageTiles = "tiles";

enum EParamStorage
{
    eParamStorageScanLine = 0,
    eParamStorageTiles
};
}
}
}
}

#endif
