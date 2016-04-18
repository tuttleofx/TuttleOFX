#include "Jpeg2000WriterPlugin.hpp"
#include "Jpeg2000WriterProcess.hpp"
#include "Jpeg2000WriterDefinitions.hpp"

#include <openjpeg/J2KWriter.hpp>

#include <tuttle/plugin/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <ofxsImageEffect.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace tuttle
{
namespace plugin
{
namespace jpeg2000
{
namespace writer
{

using namespace boost::filesystem;
using namespace tuttle::io;
using namespace boost::gil;

Jpeg2000WriterPlugin::Jpeg2000WriterPlugin(OfxImageEffectHandle handle)
    : WriterPlugin(handle)
{
    _paramCineProfil = fetchChoiceParam(kParamCinemaProfil);
    _paramLossless = fetchBooleanParam(kParamLossless);
}

Jpeg2000ProcessParams Jpeg2000WriterPlugin::getProcessParams(const OfxTime time)
{
    Jpeg2000ProcessParams params;
    params._filepath = getAbsoluteFilenameAt(time);
    params._bitDepth = (ETuttlePluginBitDepth) this->_paramBitDepth->getValue();
    params._cineProfil = _paramCineProfil->getValue();
    params._lossless = _paramLossless->getValue();

    return params;
}

void Jpeg2000WriterPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    WriterPlugin::changedParam(args, paramName);
    if(paramName == kParamCinemaProfil && args.reason == OFX::eChangeUserEdit)
    {
        if(_paramCineProfil->getValue() != 0)
        {
            _paramLossless->setEnabled(false);
            // DCI needs 12b
            _paramBitDepth->setValue(1);
            _paramBitDepth->setEnabled(false);
        }
        else
        {
            _paramLossless->setEnabled(true);
            _paramBitDepth->setEnabled(true);
        }
    }
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void Jpeg2000WriterPlugin::render(const OFX::RenderArguments& args)
{
    WriterPlugin::render(args);

    doGilRender<Jpeg2000WriterProcess>(*this, args);
}
}
}
}
}
