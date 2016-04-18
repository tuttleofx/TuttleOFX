#ifndef _TUTTLE_PLUGIN_JPEG2000_READER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_JPEG2000_READER_PLUGIN_HPP_

#include <tuttle/ioplugin/context/ReaderPlugin.hpp>
#include "openjpeg/J2KReader.hpp"

namespace tuttle
{
namespace plugin
{
namespace jpeg2000
{
namespace reader
{

struct Jpeg2000ReaderProcessParams
{
    std::string _paramFilepath; ///< filepath
};

/**
 * @brief Jpeg2000 plugin
 */
class Jpeg2000ReaderPlugin : public ReaderPlugin
{
public:
    Jpeg2000ReaderPlugin(OfxImageEffectHandle handle);
    ~Jpeg2000ReaderPlugin();

public:
    Jpeg2000ReaderProcessParams getProcessParams(const OfxTime time);
    void render(const OFX::RenderArguments& args);
    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);
    bool getRegionOfDefinition(const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod);
    void getClipPreferences(OFX::ClipPreferencesSetter& clipPreferences);

    struct FileInfo
    {
        FileInfo()
            : _time(0)
            , _failed(true)
            , _width(0)
            , _height(0)
            , _components(0)
            , _precision(0)
            , _precisionType(OFX::eBitDepthNone)
        {
        }
        OfxTime _time;
        bool _failed;
        std::size_t _width;
        std::size_t _height;
        std::size_t _components;
        std::size_t _precision;
        OFX::EBitDepth _precisionType;
    };

    FileInfo retrieveFileInfo(const OfxTime time);

public:
    tuttle::io::J2KReader _reader;

private:
    mutable FileInfo _fileInfos;
};
}
}
}
}

#endif
