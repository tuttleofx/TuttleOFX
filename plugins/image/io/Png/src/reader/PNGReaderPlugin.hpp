#ifndef _PNG_READER_PLUGIN_HPP_
#define _PNG_READER_PLUGIN_HPP_

#include <tuttle/plugin/context/ReaderPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace reader {

struct PNGReaderProcessParams
{
	std::string _filepath;       ///< filepath
};

/**
 * @brief Png reader
 *
 */
class PNGReaderPlugin : public ReaderPlugin
{
public:
	PNGReaderPlugin( OfxImageEffectHandle handle );

public:
	PNGReaderProcessParams getProcessParams(const OfxTime time);
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
