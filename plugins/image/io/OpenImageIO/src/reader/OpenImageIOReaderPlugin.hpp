#ifndef _TUTTLE_PLUGIN_OPENIMAGEIO_READER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_OPENIMAGEIO_READER_PLUGIN_HPP_

#include <tuttle/plugin/context/ReaderPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace reader {

struct OpenImageIOReaderProcessParams
{
	std::string _filepath;       ///< filepath
};

/**
 * @brief OpenImageIO reader
 *
 */
class OpenImageIOReaderPlugin : public ReaderPlugin
{
public:
	OpenImageIOReaderPlugin( OfxImageEffectHandle handle );

public:
	OpenImageIOReaderProcessParams getProcessParams( const OfxTime time );
	void                           changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool                           getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void                           getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	
	void                           render( const OFX::RenderArguments& args );
};

}
}
}
}

#endif
