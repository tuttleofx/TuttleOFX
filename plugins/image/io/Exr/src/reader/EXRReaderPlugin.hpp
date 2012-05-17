#ifndef _TUTTLE_PLUGIN_EXR_READER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_EXR_READER_PLUGIN_HPP_

#include <tuttle/plugin/context/ReaderPlugin.hpp>
#include <ImfInputFile.h>

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

struct EXRReaderProcessParams
{
	std::string _filepath;       ///< filepath
	int         _fileComponents; ///< File Component type
	int         _outComponents;  ///< Components type
	int         _redChannelIndex;
	int         _greenChannelIndex;
	int         _blueChannelIndex;
	int         _alphaChannelIndex;
};

/**
 * @brief Exr reader
 */
class EXRReaderPlugin : public ReaderPlugin
{
public:
	EXRReaderPlugin( OfxImageEffectHandle handle );
	EXRReaderProcessParams getProcessParams( const OfxTime time );

public:
	void changedParam         ( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences   ( OFX::ClipPreferencesSetter& clipPreferences );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );

	void render( const OFX::RenderArguments& args );

	const std::vector<std::string>&       channelNames() const  { return _vChannelNames; }
	const std::vector<OFX::ChoiceParam*>& channelChoice() const { return _vChannelChoice; }

private:
	void updateCombos();

protected:
	std::vector<OFX::ChoiceParam*> _vChannelChoice;  ///< Channel choice
	std::vector<std::string>       _vChannelNames;   ///< Channel names
	OFX::ChoiceParam*              _outComponents;   ///< Components list
	OFX::ChoiceParam*              _redComponents;   ///< index of Red components
	OFX::ChoiceParam*              _greenComponents; ///< index of Green components
	OFX::ChoiceParam*              _blueComponents;  ///< index of Blue components
	OFX::ChoiceParam*              _alphaComponents; ///< index of Alpha components
	int                            _channels;        ///< number of channels in file
};

}
}
}
}

#endif

