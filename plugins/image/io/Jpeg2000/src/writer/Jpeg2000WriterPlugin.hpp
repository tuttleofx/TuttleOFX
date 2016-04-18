#ifndef _TUTTLE_PLUGIN_JPEG2000_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_JPEG2000_WRITER_PLUGIN_HPP_

#include "Jpeg2000WriterDefinitions.hpp"
#include <tuttle/ioplugin/context/WriterPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace jpeg2000
{
namespace writer
{

struct Jpeg2000ProcessParams
{
    std::string _filepath;           ///< filepath
    ETuttlePluginBitDepth _bitDepth; ///< Precision (in bits)
    int _cineProfil;                 ///< Cinema Profile
    bool _lossless;                  ///< Lossless compression
};

/**
 * @brief Jpeg2000 plugin
 */
class Jpeg2000WriterPlugin : public WriterPlugin
{
public:
    Jpeg2000WriterPlugin(OfxImageEffectHandle handle);

public:
    Jpeg2000ProcessParams getProcessParams(const OfxTime time);

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    void render(const OFX::RenderArguments& args);

protected:
    OFX::ChoiceParam* _paramCineProfil;
    OFX::BooleanParam* _paramLossless;
};
}
}
}
}

#endif
