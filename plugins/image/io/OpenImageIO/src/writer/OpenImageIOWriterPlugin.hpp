#ifndef _OPENIMAGEIO_WRITER_PLUGIN_HPP_
#define _OPENIMAGEIO_WRITER_PLUGIN_HPP_

#include <tuttle/plugin/context/WriterPlugin.hpp>
#include <OpenImageIO/typedesc.h>

#include "OpenImageIOWriterDefinitions.hpp"

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
	void                           render( const OFX::RenderArguments& args );

public:
	OFX::ChoiceParam* _components;     ///< Choose components RGBA/RGB
};

}
}
}
}

#endif
