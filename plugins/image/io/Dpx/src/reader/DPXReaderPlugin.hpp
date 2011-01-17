#ifndef _TUTTLE_PLUGIN_DPX_READER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_DPX_READER_PLUGIN_HPP_

#include <tuttle/plugin/context/ReaderPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace reader {

struct DPXReaderProcessParams
{
	std::string _filepath;      ///< filepath
	bool _flip;
};

/**
 * @brief Dpx reader
 */
class DPXReaderPlugin : public ReaderPlugin
{
public:
	DPXReaderPlugin( OfxImageEffectHandle handle );

public:
	DPXReaderProcessParams getProcessParams( const OfxTime time );

	void                   changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool                   getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void                   getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

	void              render( const OFX::RenderArguments& args );
};

}
}
}
}

#endif

