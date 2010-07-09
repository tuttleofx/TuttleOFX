#ifndef _JPEG_READER_PLUGIN_HPP_
#define _JPEG_READER_PLUGIN_HPP_

#include <tuttle/plugin/context/ReaderPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace reader {

struct JpegReaderProcessParams
{
	std::string _filepath;       ///< filepath
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
	JpegReaderProcessParams getProcessParams(const OfxTime time);
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
