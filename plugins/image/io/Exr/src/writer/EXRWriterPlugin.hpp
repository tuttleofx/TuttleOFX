#ifndef _TUTTLE_PLUGIN_EXRWRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_EXRWRITER_PLUGIN_HPP_

#include "EXRWriterDefinitions.hpp"
#include <tuttle/ioplugin/context/WriterPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace exr
{
namespace writer
{

struct EXRWriterProcessParams
{
    std::string _filepath;
    ETuttlePluginFileBitDepth _fileBitDepth;
    ETuttlePluginComponents _componentsType;
    EParamStorage _storageType;
    EParamCompression _compression;
};

/**
 * @brief
 *
 */
class EXRWriterPlugin : public WriterPlugin
{
public:
    EXRWriterPlugin(OfxImageEffectHandle handle);

public:
    EXRWriterProcessParams getProcessParams(const OfxTime time);
    void render(const OFX::RenderArguments& args);

protected:
    OFX::ChoiceParam* _paramComponentsType;
    OFX::ChoiceParam* _paramStorageType;
    OFX::ChoiceParam* _paramFileBitDepth;
    OFX::ChoiceParam* _paramCompression;
};
}
}
}
}

#endif
