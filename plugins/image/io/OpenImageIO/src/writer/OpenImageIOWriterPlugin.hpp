#ifndef _TUTTLE_PLUGIN_OPENIMAGEIO_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_OPENIMAGEIO_WRITER_PLUGIN_HPP_

#include "OpenImageIOWriterDefinitions.hpp"

#include <tuttle/plugin/context/WriterPlugin.hpp>

#include <OpenImageIO/typedesc.h>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace writer {

struct OpenImageIOWriterProcessParams
{
	std::string _filepath;        ///< filepath
	EParamComponents _components; ///< Force RGB
	TypeDesc _bitDepth;           ///< Output bit depth
};

/**
 * @brief OpenImageIO writer
 */
class OpenImageIOWriterPlugin : public WriterPlugin
{
public:
	OpenImageIOWriterPlugin( OfxImageEffectHandle handle );

public:
	OpenImageIOWriterProcessParams getProcessParams( const OfxTime time );
	void                      render( const OFX::RenderArguments& args );

public:
	OFX::ChoiceParam* _components;     ///< Choose components RGBA/RGB
};

}
}
}
}

#endif
