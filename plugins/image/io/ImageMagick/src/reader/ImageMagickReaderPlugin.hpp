#ifndef _TUTTLE_PLUGIN_IMAGEMAGICK_READER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_IMAGEMAGICK_READER_PLUGIN_HPP_

#include <tuttle/plugin/context/ReaderPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace imagemagick {
namespace reader {

struct ImageMagickReaderProcessParams
{
	std::string _filepath;       ///< filepath
	bool _flip;
};

/**
 * @brief ImageMagick reader
 *
 */
class ImageMagickReaderPlugin : public ReaderPlugin
{
public:
	ImageMagickReaderPlugin( OfxImageEffectHandle handle );

public:
	ImageMagickReaderProcessParams getProcessParams( const OfxTime time );
	void                           render( const OFX::RenderArguments& args );
	void                           changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool                           getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void                           getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
};

}
}
}
}

#endif
