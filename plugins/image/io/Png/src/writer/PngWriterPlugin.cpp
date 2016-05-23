#include "PngWriterPlugin.hpp"
#include "PngWriterProcess.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace png
{
namespace writer
{

using namespace boost::gil;

PngWriterPlugin::PngWriterPlugin(OfxImageEffectHandle handle)
    : WriterPlugin(handle)
{
    _paramOutputComponents = fetchChoiceParam(kTuttlePluginChannel);
}

PngWriterProcessParams PngWriterPlugin::getProcessParams(const OfxTime time)
{
    PngWriterProcessParams params;

    params._filepath = getAbsoluteFilenameAt(time);
    params._components = static_cast<ETuttlePluginComponents>(this->_paramOutputComponents->getValue());
    params._bitDepth = static_cast<ETuttlePluginBitDepth>(this->_paramBitDepth->getValue());

    return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void PngWriterPlugin::render(const OFX::RenderArguments& args)
{
    WriterPlugin::render(args);

    doGilRender<PngWriterProcess>(*this, args);
}
}
}
}
}
