#ifndef _TUTTLE_PLUGIN_TURBOJPEG_READER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_TURBOJPEG_READER_PLUGIN_HPP_

#include "TurboJpegReaderDefinitions.hpp"

#include <tuttle/ioplugin/context/ReaderPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace turboJpeg {
namespace reader {

struct TurboJpegReaderProcessParams
{
	std::string            filepath;
	ETurboJpegOptimization optimization;
	bool                   fastUpsampling;
};

/**
 * @brief TurboJpeg plugin
 */
class TurboJpegReaderPlugin : public ReaderPlugin
{
public:
    TurboJpegReaderPlugin( OfxImageEffectHandle handle );

public:
	TurboJpegReaderProcessParams getProcessParams( const OfxTime time ) const;

	void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

	void render( const OFX::RenderArguments &args );

public:
	OFX::ChoiceParam*    _optimization;   ///< TurboJpeg SIMD optimization
	OFX::BooleanParam*   _fastUpsampling; ///< TurboJpeg fast upsampling for U,V channels
	
};

}
}
}
}

#endif
