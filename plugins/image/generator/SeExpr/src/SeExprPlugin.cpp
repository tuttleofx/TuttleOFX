#include "SeExprPlugin.hpp"
#include "SeExprProcess.hpp"
#include "SeExprDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <fstream>

namespace tuttle
{
namespace plugin
{
namespace seExpr
{

SeExprPlugin::SeExprPlugin(OfxImageEffectHandle handle)
    : GeneratorPlugin(handle)
{
    _paramInput = fetchChoiceParam(kParamChooseInput);
    _paramCode = fetchStringParam(kParamSeExprCode);
    _paramFile = fetchStringParam(kTuttlePluginFilename);
    _paramTextureOffset = fetchDouble2DParam(kParamTextureOffset);

    changedParam(_instanceChangedArgs, kParamChooseInput);
}

SeExprProcessParams<SeExprPlugin::Scalar> SeExprPlugin::getProcessParams(const OfxPointD& renderScale) const
{
    SeExprProcessParams<Scalar> params;
    params._inputType = static_cast<EParamChooseInput>(_paramInput->getValue());
    switch(params._inputType)
    {
        case eParamChooseInputCode:
        {
            params._code = _paramCode->getValue();
            break;
        }
        case eParamChooseInputFile:
        {
            try
            {
                std::ifstream myfile(_paramFile->getValue().c_str(), std::ifstream::in);
                if(myfile.is_open())
                {
                    while(myfile.good())
                    {
                        std::string line;
                        getline(myfile, line);
                        params._code += line + "\n";
                    }
                    myfile.close();
                }
            }
            catch(...)
            {
                // BOOST_THROW_EXCEPTION( exception::File() << "unable to read file " + _paramFile->getValue() );
            }
            break;
        }
    }

    OfxPointD textureOffset = _paramTextureOffset->getValue();
    params._paramTextureOffset.x = textureOffset.x * renderScale.x;
    params._paramTextureOffset.y = textureOffset.y * renderScale.y;

    return params;
}

void SeExprPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    GeneratorPlugin::changedParam(args, paramName);

    if(paramName == kParamChooseInput)
    {
        EParamChooseInput input = static_cast<EParamChooseInput>(_paramInput->getValue());
        switch(input)
        {
            case eParamChooseInputCode:
            {
                _paramCode->setIsSecretAndDisabled(false);
                _paramFile->setIsSecretAndDisabled(true);
                break;
            }
            case eParamChooseInputFile:
            {
                _paramCode->setIsSecretAndDisabled(true);
                _paramFile->setIsSecretAndDisabled(false);
                break;
            }
        }
    }

    if(paramName == kParamSeExprCode)
    {
        _paramInput->setValue(eParamChooseInputCode);
    }

    if(paramName == kTuttlePluginFilename)
    {
        _paramInput->setValue(eParamChooseInputFile);
    }
}

void SeExprPlugin::getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences)
{
    GeneratorPlugin::getClipPreferences(clipPreferences);
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void SeExprPlugin::render(const OFX::RenderArguments& args)
{
    doGilRender<SeExprProcess>(*this, args);
}
}
}
}
