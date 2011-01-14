#ifndef _TUTTLE_PLUGIN_DPXWRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_DPXWRITER_PLUGIN_HPP_

#include <tuttle/plugin/context/WriterPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

struct DPXWriterProcessParams
{
	std::string _filepath;       ///< filepath
	int _bitDepth;               ///< Output bit depth
	int _componentsType;         ///< Components type
	bool _compressed;            ///< Bit streaming
	bool _flip;
};

/**
 * @brief
 *
 */
class DPXWriterPlugin : public WriterPlugin
{
public:
	DPXWriterPlugin( OfxImageEffectHandle handle );

public:
	DPXWriterProcessParams getProcessParams( const OfxTime time );
	
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	void render( const OFX::RenderArguments& args );

protected:
	OFX::ChoiceParam*    _componentsType;  ///< Dpx components type
	OFX::BooleanParam*   _compressed;      ///< Dpx is bit streamed
};

}
}
}
}

#endif
