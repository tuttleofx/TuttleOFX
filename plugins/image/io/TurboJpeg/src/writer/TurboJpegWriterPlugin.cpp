#include "TurboJpegWriterPlugin.hpp"
#include "TurboJpegWriterProcess.hpp"
#include "TurboJpegWriterDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace turboJpeg
{
namespace writer
{

TurboJpegWriterPlugin::TurboJpegWriterPlugin(OfxImageEffectHandle handle)
    : WriterPlugin(handle)
{
    _paramPremult = fetchBooleanParam(kParamPremultiplied);
    _paramQuality = fetchIntParam(kParamQuality);
    _paramSubsampling = fetchChoiceParam(kParamSubsampling);
    _paramOptimization = fetchChoiceParam(kParamOptimization);
}

TurboJpegWriterProcessParams TurboJpegWriterPlugin::getProcessParams(const OfxTime time)
{
    TurboJpegWriterProcessParams params;

    params.filepath = getAbsoluteFilenameAt(time);
    params.quality = this->_paramQuality->getValue();
    params.premult = this->_paramPremult->getValue();
    params.optimization = static_cast<ETurboJpegOptimization>(_paramOptimization->getValue());
    params.subsampling = static_cast<ETurboJpegSubsampling>(_paramSubsampling->getValue());
    return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void TurboJpegWriterPlugin::render(const OFX::RenderArguments& args)
{
    WriterPlugin::render(args);
    doGilRender<TurboJpegWriterProcess>(*this, args);
}
}
}
}
}
