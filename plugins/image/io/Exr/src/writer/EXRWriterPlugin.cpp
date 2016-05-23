#include "EXRWriterDefinitions.hpp"
#include "EXRWriterPlugin.hpp"
#include "EXRWriterProcess.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace exr
{
namespace writer
{

using namespace boost::gil;

EXRWriterPlugin::EXRWriterPlugin(OfxImageEffectHandle handle)
    : WriterPlugin(handle)
{
    _paramComponentsType = fetchChoiceParam(kTuttlePluginChannel);
    _paramStorageType = fetchChoiceParam(kParamStorageType);

    _paramFileBitDepth = fetchChoiceParam(kParamFileBitDepth);
    _paramCompression = fetchChoiceParam(kParamCompression);
}

EXRWriterProcessParams EXRWriterPlugin::getProcessParams(const OfxTime time)
{
    EXRWriterProcessParams params;

    params._filepath = getAbsoluteFilenameAt(time);
    params._fileBitDepth = (ETuttlePluginFileBitDepth) this->_paramFileBitDepth->getValue();
    params._componentsType = (ETuttlePluginComponents)_paramComponentsType->getValue();
    params._storageType = (EParamStorage)_paramStorageType->getValue();
    params._compression = (EParamCompression)_paramCompression->getValue();

    return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void EXRWriterPlugin::render(const OFX::RenderArguments& args)
{
    WriterPlugin::render(args);

    doGilRender<EXRWriterProcess>(*this, args);
}
}
}
}
}
