#ifndef _TUTTLE_PLUGIN_EXR_READER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_EXR_READER_PLUGIN_HPP_

#include <tuttle/ioplugin/context/ReaderPlugin.hpp>
#include <ImfInputFile.h>

namespace tuttle
{
namespace plugin
{
namespace exr
{
namespace reader
{

struct EXRReaderProcessParams
{
    std::string _filepath; ///< filepath
    int _fileNbChannels;   ///< Number of channels in the Exr file
    int _userNbComponents; ///< Number of channel asked by user
    int _outComponents;    ///< Components type

    int _redChannelIndex;
    int _greenChannelIndex;
    int _blueChannelIndex;
    int _alphaChannelIndex;
    bool _displayWindow;
};

/**
 * @brief Exr reader
 */
class EXRReaderPlugin : public ReaderPlugin
{
public:
    EXRReaderPlugin(OfxImageEffectHandle handle);
    EXRReaderProcessParams getProcessParams(const OfxTime time);

public:
    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);
    void getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences);
    bool getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod);

    void render(const OFX::RenderArguments& args);

    const std::vector<std::string>& channelNames() const { return _channelNames; }
    const std::vector<OFX::ChoiceParam*>& channelChoice() const { return _paramsChannelChoice; }

private:
    void updateCombos();

protected:
    std::vector<OFX::ChoiceParam*> _paramsChannelChoice; ///< Channel choice
    std::vector<std::string> _channelNames;              ///< Channel names
    OFX::ChoiceParam* _paramOutComponents;               ///< Components list
    OFX::ChoiceParam* _paramRedComponents;               ///< index of Red components
    OFX::ChoiceParam* _paramGreenComponents;             ///< index of Green components
    OFX::ChoiceParam* _paramBlueComponents;              ///< index of Blue components
    OFX::ChoiceParam* _paramAlphaComponents;             ///< index of Alpha components
    OFX::ChoiceParam* _paramOutputData;                  ///< Output data
    OFX::ChoiceParam* _paramFileCompression;
    OFX::ChoiceParam* _paramFileBitDepth;
    float _par; ///< pixel aspect ratio
};
}
}
}
}

#endif
