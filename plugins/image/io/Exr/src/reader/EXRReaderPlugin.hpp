#ifndef _EXR_READER_PLUGIN_HPP_
#define _EXR_READER_PLUGIN_HPP_

#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>
#include <ImfInputFile.h>
#include <tuttle/plugin/PluginException.hpp>
#include <tuttle/plugin/FilenameManager.hpp>

namespace bgil = boost::gil;

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

struct EXRReaderParams
{
	std::string _filepath;      ///< filepath
	int _outComponents;         ///< Components type
};

/**
 * @brief Exr reader
 */
class EXRReaderPlugin : public OFX::ImageEffect
{
public:
	EXRReaderPlugin( OfxImageEffectHandle handle );
	OFX::Clip* getDstClip() const;
	EXRReaderParams getParams(const OfxTime time);
public:
	virtual void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	bool getTimeDomain( OfxRangeD& range );
	const std::vector<std::string>&       channelNames() const  { return _vChannelNames; }
	const std::vector<OFX::ChoiceParam*>& channelChoice() const { return _vChannelChoice; }

protected:
	std::vector<OFX::ChoiceParam*> _vChannelChoice;  ///< Channel choice
	std::vector<std::string>       _vChannelNames;   ///< Channel names
	bgil::point2<ptrdiff_t>        _imageDims;       ///< Image file dimensions
	OFX::StringParam*              _filepath;        ///< File path
	OFX::ChoiceParam*              _outComponents;   ///< Components list
	FilenameManager                _fPattern;        ///< Filename pattern manager
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _dstClip;                             ///< Destination image clip
};

}
}
}
}

#endif

