#ifndef _OPENIMAGEIO_WRITER_PLUGIN_HPP_
#define _OPENIMAGEIO_WRITER_PLUGIN_HPP_

#include <tuttle/plugin/context/WriterPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace writer {

using namespace boost::gil;

struct OpenImageIOWriterProcessParams
{
	std::string _filepath;      ///< filepath
	bool        _outputRGB;		///< Force RGB
	int         _bitDepth;      ///< Output bit depth
};

/**
 * @brief OpenImageIO writer
 */
class OpenImageIOWriterPlugin : public WriterPlugin
{
public:
	OpenImageIOWriterPlugin( OfxImageEffectHandle handle );

public:
	OpenImageIOWriterProcessParams getParams(const OfxTime time);
	virtual void render( const OFX::RenderArguments& args );

public:
	OFX::BooleanParam* _outputRGB;     ///< Force RGB
};

}
}
}
}

#endif
