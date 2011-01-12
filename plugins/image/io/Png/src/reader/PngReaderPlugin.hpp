#ifndef _PNG_READER_PLUGIN_HPP_
#define _PNG_READER_PLUGIN_HPP_

#include <tuttle/plugin/context/ReaderPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace reader {

struct PngReaderProcessParams
{
	std::string _filepath;       ///< filepath
	bool _flip;
};

/**
 * @brief Png reader
 *
 */
class PngReaderPlugin : public ReaderPlugin
{
public:
	PngReaderPlugin( OfxImageEffectHandle handle );

public:
	PngReaderProcessParams getProcessParams( const OfxTime time );

	void                   changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool                   getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void                   getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

	void                   render( const OFX::RenderArguments& args );
};

}
}
}
}

#endif
