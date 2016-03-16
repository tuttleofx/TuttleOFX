#include "CTLPlugin.hpp"
#include "CTLProcess.hpp"
#include "CTLDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/algorithm/string/split.hpp>

#include <fstream>
#include <boost/filesystem/path.hpp>

namespace tuttle
{
namespace plugin
{
namespace ctl
{

CTLPlugin::CTLPlugin(OfxImageEffectHandle handle)
    : ImageEffectGilPlugin(handle)
{
    _paramInput = fetchChoiceParam(kParamChooseInput);
    _paramCode = fetchStringParam(kParamCTLCode);
    _paramFile = fetchStringParam(kTuttlePluginFilename);
    _paramUpdateRender = fetchPushButtonParam(kParamChooseInputCodeUpdate);

    changedParam(_instanceChangedArgs, kParamChooseInput);
}

CTLProcessParams<CTLPlugin::Scalar> CTLPlugin::getProcessParams(const OfxPointD& renderScale) const
{
    using namespace boost::filesystem;
    CTLProcessParams<Scalar> params;
    params._inputType = static_cast<EParamChooseInput>(_paramInput->getValue());
    switch(params._inputType)
    {
        case eParamChooseInputCode:
        {
            params._module = "inputCode";
            params._code = _paramCode->getValue();
            break;
        }
        case eParamChooseInputFile:
        {
            //			std::ifstream f( _paramFile->getValue().c_str(), std::ios::in );
            //			std::getline( f, params._code, '\0' );
            //			split( params._paths, paths, is_any_of(":;"), token_compress_on );
            const path filename = path(_paramFile->getValue());
            params._filename = _paramFile->getValue();
            params._module = filename.stem().string();
            params._paths.push_back(filename.parent_path().string());
            break;
        }
    }
    return params;
}

void CTLPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    if(paramName == kParamChooseInput)
    {
        EParamChooseInput input = static_cast<EParamChooseInput>(_paramInput->getValue());
        switch(input)
        {
            case eParamChooseInputCode:
            {
                _paramCode->setIsSecretAndDisabled(false);
                _paramUpdateRender->setIsSecretAndDisabled(false);
                _paramFile->setIsSecretAndDisabled(true);
                break;
            }
            case eParamChooseInputFile:
            {
                _paramCode->setIsSecretAndDisabled(true);
                _paramUpdateRender->setIsSecretAndDisabled(true);
                _paramFile->setIsSecretAndDisabled(false);
                break;
            }
        }
    }
    else if(paramName == kParamCTLCode)
    {
        _paramInput->setValue(eParamChooseInputCode);
    }
    else if(paramName == kTuttlePluginFilename)
    {
        _paramInput->setValue(eParamChooseInputFile);
    }
}

bool CTLPlugin::getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod)
{
    OfxRectD srcRod = _clipSrc->getCanonicalRod(args.time);

    rod.x1 = srcRod.x1;
    rod.y1 = srcRod.y1;
    rod.x2 = srcRod.x2;
    rod.y2 = srcRod.y2;

    return false;
}

bool CTLPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    //	CTLProcessParams<Scalar> params = getProcessParams();
    //	if( params._in == params._out )
    //	{
    //		identityClip = _clipSrc;
    //		identityTime = args.time;
    //		return true;
    //	}
    return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void CTLPlugin::render(const OFX::RenderArguments& args)
{
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth bitDepth = _clipDst->getPixelDepth();
    OFX::EPixelComponent components = _clipDst->getPixelComponents();

    switch(components)
    {
        case OFX::ePixelComponentRGBA:
        {
            doGilRender<CTLProcess, false, boost::gil::rgba_layout_t>(*this, args, bitDepth);
            return;
        }
        case OFX::ePixelComponentRGB:
        case OFX::ePixelComponentAlpha:
        case OFX::ePixelComponentCustom:
        case OFX::ePixelComponentNone:
        {
            BOOST_THROW_EXCEPTION(exception::Unsupported() << exception::user() + "Pixel components (" +
                                                                  mapPixelComponentEnumToString(components) +
                                                                  ") not supported by the plugin.");
        }
    }
    BOOST_THROW_EXCEPTION(exception::Unknown());
}
}
}
}
