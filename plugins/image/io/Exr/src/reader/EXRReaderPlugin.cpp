#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPlugin.hpp"
#include "EXRReaderProcess.hpp"

#include <tuttle/ioplugin/context/ReaderPlugin.hpp>

#include <ImfInputFile.h>
#include <ImathBox.h>
#include <ImfChannelList.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace tuttle
{
namespace plugin
{
namespace exr
{
namespace reader
{

namespace bfs = boost::filesystem;
using namespace Imf;
using namespace boost::gil;

EXRReaderPlugin::EXRReaderPlugin(OfxImageEffectHandle handle)
    : ReaderPlugin(handle)
    , _par(1.0)
{
    _paramOutComponents = fetchChoiceParam(kTuttlePluginChannel);

    _paramRedComponents = fetchChoiceParam(kParamOutputRedIs);
    _paramGreenComponents = fetchChoiceParam(kParamOutputGreenIs);
    _paramBlueComponents = fetchChoiceParam(kParamOutputBlueIs);
    _paramAlphaComponents = fetchChoiceParam(kParamOutputAlphaIs);

    _paramsChannelChoice.push_back(_paramRedComponents);
    _paramsChannelChoice.push_back(_paramGreenComponents);
    _paramsChannelChoice.push_back(_paramBlueComponents);
    _paramsChannelChoice.push_back(_paramAlphaComponents);

    _paramOutputData = fetchChoiceParam(kParamOutputData);

    _paramFileCompression = fetchChoiceParam(kParamCompression);
    _paramFileBitDepth = fetchChoiceParam(kParamFileBitDepth);
}

EXRReaderProcessParams EXRReaderPlugin::getProcessParams(const OfxTime time)
{
    EXRReaderProcessParams params;

    params._filepath = getAbsoluteFilenameAt(time);
    params._outComponents = _paramOutComponents->getValue();
    params._fileNbChannels = _channelNames.size();
    params._userNbComponents = 0;

    switch(EParamReaderChannel(params._outComponents))
    {
        case eParamReaderChannelGray:
        {
            params._userNbComponents = 1;
            break;
        }
        case eParamReaderChannelRGB:
        {
            params._userNbComponents = 3;
            break;
        }
        case eParamReaderChannelAuto:
        case eParamReaderChannelRGBA:
        {
            params._userNbComponents = 4;
            break;
        }
    }

    params._displayWindow = (_paramOutputData->getValue() == 0);

    return params;
}

void EXRReaderPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    if(paramName == kTuttlePluginFilename)
    {
        ReaderPlugin::changedParam(args, paramName);
        updateCombos();
    }
    else if(paramName == kTuttlePluginChannel)
    {
        switch(_paramOutComponents->getValue())
        {
            case eParamReaderChannelGray:
            {
                for(std::size_t j = 0; j < _paramsChannelChoice.size() - 1; ++j)
                {
                    _paramsChannelChoice[j]->setIsSecretAndDisabled(true);
                }
                _paramsChannelChoice[3]->setIsSecretAndDisabled(false);
                break;
            }
            case eParamReaderChannelRGB:
            {
                for(std::size_t j = 0; j < _paramsChannelChoice.size() - 1; ++j)
                {
                    _paramsChannelChoice[j]->setIsSecretAndDisabled(false);
                }
                _paramsChannelChoice[3]->setIsSecretAndDisabled(true);
                break;
            }
            case eParamReaderChannelAuto:
            case eParamReaderChannelRGBA:
            {
                for(std::size_t j = 0; j < _paramsChannelChoice.size(); ++j)
                {
                    _paramsChannelChoice[j]->setIsSecretAndDisabled(false);
                }
                break;
            }
        }
    }
    else
    {
        ReaderPlugin::changedParam(args, paramName);
    }
}

void EXRReaderPlugin::updateCombos()
{
    const std::string filepath(getAbsoluteFirstFilename());
    if(!bfs::exists(filepath))
        return;

    // read dims
    InputFile in(filepath.c_str());
    const Header& h = in.header();
    const ChannelList& cl = h.channels();

    _par = h.pixelAspectRatio();

    Imf::Compression compression = h.compression();

    switch(compression)
    {
        case RLE_COMPRESSION:
            _paramFileCompression->setValue(eParamCompression_RLE);
            break;
        case ZIPS_COMPRESSION:
            _paramFileCompression->setValue(eParamCompression_ZIPS);
            break;
        case ZIP_COMPRESSION:
            _paramFileCompression->setValue(eParamCompression_ZIP);
            break;
        case PIZ_COMPRESSION:
            _paramFileCompression->setValue(eParamCompression_PIZ);
            break;
        case PXR24_COMPRESSION:
            _paramFileCompression->setValue(eParamCompression_PXR24);
            break;
        case B44_COMPRESSION:
            _paramFileCompression->setValue(eParamCompression_B44);
            break;
        case B44A_COMPRESSION:
            _paramFileCompression->setValue(eParamCompression_B44A);
            break;
        case NO_COMPRESSION:
            _paramFileCompression->setValue(eParamCompression_None);
            break;
        default:
            _paramFileCompression->setValue(eParamCompression_None);
            TUTTLE_LOG_WARNING("EXRReader: Unknown compression " << compression);
            break;
    }

    // Hide output channel selection till we don't select a channel.
    for(std::size_t i = 0; i < _paramsChannelChoice.size(); ++i)
    {
        //_vChannelChoice[i]->setIsSecret( true );
        _paramsChannelChoice[i]->resetOptions();
    }
    _channelNames.clear();
    for(ChannelList::ConstIterator it = cl.begin(); it != cl.end(); ++it)
    {
        _channelNames.push_back(it.name());
        // TUTTLE_LOG_VAR( it.name() );
        for(std::size_t j = 0; j < _paramsChannelChoice.size(); ++j)
        {
            _paramsChannelChoice[j]->appendOption(it.name());
        }
    }
    switch(_channelNames.size())
    {
        case 1:
        {
            for(std::size_t j = 0; j < _paramsChannelChoice.size(); j++)
                _paramsChannelChoice.at(j)->setValue(0);
            break;
        }
        case 3:
        {
            for(std::size_t j = 0; j < _paramsChannelChoice.size() - 1; j++)
                _paramsChannelChoice.at(j)->setValue(2 - j);
            _paramsChannelChoice.at(3)->setValue(0);
            break;
        }
        case 4:
        {
            for(std::size_t j = 0; j < _paramsChannelChoice.size(); j++)
                _paramsChannelChoice.at(j)->setValue(3 - j);
            break;
        }
    }

    const std::string& firstChannelName = _channelNames[_paramsChannelChoice[0]->getValue()];
    Imf::PixelType firstChannelType = cl.find(firstChannelName.c_str()).channel().type;
    switch(firstChannelType)
    {
        case Imf::UINT:
            _paramFileBitDepth->setValue(eTuttlePluginFileBitDepth32);
            break;
        case Imf::HALF:
            _paramFileBitDepth->setValue(eTuttlePluginFileBitDepth16f);
            break;
        case Imf::FLOAT:
            _paramFileBitDepth->setValue(eTuttlePluginFileBitDepth32f);
            break;
        default:
            _paramFileBitDepth->setValue(eTuttlePluginFileBitDepthNone);
            TUTTLE_LOG_WARNING("EXRReader: Unknown bit depth " << firstChannelType);
            break;
    }
}

void EXRReaderPlugin::getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences)
{
    ReaderPlugin::getClipPreferences(clipPreferences);

    if(getExplicitChannelConversion() == eParamReaderChannelAuto)
    {
        switch(_channelNames.size())
        {
            case 1:
            {
                if(OFX::getImageEffectHostDescription()->supportsPixelComponent(OFX::ePixelComponentAlpha))
                    clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentAlpha);
                break;
            }
            case 2:
            case 3:
            {
                if(OFX::getImageEffectHostDescription()->supportsPixelComponent(OFX::ePixelComponentRGB))
                    clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGB);
                break;
            }
            default:
                // case 4:
                {
                    if(OFX::getImageEffectHostDescription()->supportsPixelComponent(OFX::ePixelComponentRGBA))
                        clipPreferences.setClipComponents(*this->_clipDst, OFX::ePixelComponentRGBA);
                    break;
                }
        }
    }

    clipPreferences.setPixelAspectRatio(*this->_clipDst, _par);
}

bool EXRReaderPlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod)
{
    const std::string filepath(getAbsoluteFilenameAt(args.time));
    if(!bfs::exists(filepath))
        return false;

    try
    {
        InputFile in(filepath.c_str());
        const Header& h = in.header();
        const Imath::Box2i displayWindow(h.displayWindow());
        // Exr is top to bottom and OpenFX is bottom to top.
        const double height = (displayWindow.max.y - displayWindow.min.y) + 1;

        if(_paramOutputData->getValue() == 0)
        {
            rod.x1 = displayWindow.min.x;
            rod.x2 = displayWindow.max.x + 1;
            rod.y1 = height - (displayWindow.max.y + 1);
            rod.y2 = height - displayWindow.min.y;
        }
        else
        {
            const Imath::Box2i dataWindow(h.dataWindow());

            //			TUTTLE_LOG_INFO( "ExrReaderPlugin: displayWindow: " << displayWindow.min.x << ", " << displayWindow.min.y
            //<< ", " << displayWindow.max.x << ", " << displayWindow.max.y );
            //			TUTTLE_LOG_INFO( "ExrReaderPlugin: dataWindow: " << h.dataWindow().min.x << ", " << h.dataWindow().min.y
            //<< ", " << h.dataWindow().max.x << ", " << h.dataWindow().max.y );

            rod.x1 = dataWindow.min.x;
            rod.x2 = dataWindow.max.x + 1;
            rod.y1 = height - (dataWindow.max.y + 1);
            rod.y2 = height - dataWindow.min.y;
        }
        rod.x1 *= h.pixelAspectRatio();
        rod.x2 *= h.pixelAspectRatio();

        rod.x1 *= args.renderScale.x;
        rod.x2 *= args.renderScale.x;
        rod.y1 *= args.renderScale.y;
        rod.y2 *= args.renderScale.y;
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(exception::FileInSequenceNotExist() << exception::user("EXR: Unable to open file.")
                                                                  << exception::filename(getAbsoluteFilenameAt(args.time)));
    }
    return true;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void EXRReaderPlugin::render(const OFX::RenderArguments& args)
{
    ReaderPlugin::render(args);
    doGilRender<EXRReaderProcess>(*this, args);
}
}
}
}
}
