#include "OpenImageIOWriterDefinitions.hpp"
#include "OpenImageIOWriterPlugin.hpp"
#include "OpenImageIOWriterProcess.hpp"

#include <imageio.h>

namespace tuttle
{
namespace plugin
{
namespace openImageIO
{
namespace writer
{

OpenImageIOWriterPlugin::OpenImageIOWriterPlugin(OfxImageEffectHandle handle)
    : WriterPlugin(handle)
{
    _components = fetchChoiceParam(kTuttlePluginChannel);
    _orientation = fetchChoiceParam(kParamOutputOrientation);
    _quality = fetchIntParam(kParamOutputQuality);
    _storageType = fetchChoiceParam(kParamOutputStorageType);
    _paramSubsampling = fetchChoiceParam(kParamOutputSubsampling);
    _compression = fetchChoiceParam(kParamCompression);
}

OpenImageIOWriterProcessParams OpenImageIOWriterPlugin::getProcessParams(const OfxTime time)
{
    using namespace OpenImageIO;
    OpenImageIOWriterProcessParams params;

    params._filepath = getAbsoluteFilenameAt(time);
    params._components = static_cast<ETuttlePluginComponents>(this->_components->getValue());
    params._bitDepth = static_cast<ETuttlePluginBitDepth>(this->_paramBitDepth->getValue());
    params._storageType = static_cast<ETuttlePluginStorage>(this->_storageType->getValue());
    params._subsampling = static_cast<ETuttlePluginSubsampling>(_paramSubsampling->getValue());
    params._compression = static_cast<ETuttlePluginCompression>(_compression->getValue());
    params._quality = _quality->getValue();
    params._orientation = static_cast<int>(_orientation->getValue());
    params._premultiply = this->_paramPremult->getValue();
    return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void OpenImageIOWriterPlugin::render(const OFX::RenderArguments& args)
{
    WriterPlugin::render(args);

    doGilRender<OpenImageIOWriterProcess>(*this, args);
}
}
}
}
}
