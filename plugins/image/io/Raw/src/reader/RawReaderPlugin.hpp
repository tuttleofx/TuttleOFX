#ifndef _TUTTLE_PLUGIN_RAWREADERPLUGIN_HPP_
#define _TUTTLE_PLUGIN_RAWREADERPLUGIN_HPP_

#include <tuttle/plugin/context/ReaderPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace raw {
namespace reader {

struct RawReaderProcessParams
{
	std::string _filepath;       ///< filepath
};

/**
 * @brief Raw reader
 *
 */
class RawReaderPlugin : public ReaderPlugin
{
public:
	RawReaderPlugin( OfxImageEffectHandle handle );

public:
	RawReaderProcessParams getProcessParams(const OfxTime time);

	void updateInfos();
	
	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
};

}
}
}
}

#endif
