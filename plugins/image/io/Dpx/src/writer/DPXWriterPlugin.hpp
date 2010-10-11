#ifndef _DPXWRITER_PLUGIN_HPP_
#define _DPXWRITER_PLUGIN_HPP_

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
	virtual void           render( const OFX::RenderArguments& args );
	void                   changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

protected:
	OFX::ChoiceParam*    _componentsType;  ///< Dpx components type
	OFX::BooleanParam*   _compressed;      ///< Dpx is bit streamed
};

}
}
}
}

#endif
