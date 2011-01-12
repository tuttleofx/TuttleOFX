#ifndef _TUTTLE_PLUGIN_JPEG_READER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_JPEG_READER_PLUGIN_HPP_

#include <tuttle/plugin/context/ReaderPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace reader {

struct JpegReaderProcessParams
{
	std::string _filepath;       ///< filepath
	bool _flip;
};

/**
 * @brief Jpeg reader
 *
 */
class JpegReaderPlugin : public ReaderPlugin
{
public:
	JpegReaderPlugin( OfxImageEffectHandle handle );

public:
	JpegReaderProcessParams getProcessParams( const OfxTime time );
	
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	
	void render( const OFX::RenderArguments& args );
};

}
}
}
}

#endif
