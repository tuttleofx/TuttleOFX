#ifndef _JPEG_WRITER_PLUGIN_HPP_
#define _JPEG_WRITER_PLUGIN_HPP_

#include <tuttle/plugin/context/WriterPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace writer {

using namespace boost::gil;

struct JpegWriterProcessParams
{
	std::string _filepath;      ///< filepath
	bool        _premult;		///< Force RGB
	int         _bitDepth;      ///< Output bit depth
	int         _quality;
};

/**
 * @brief Jpeg writer
 */
class JpegWriterPlugin : public WriterPlugin
{
public:
	JpegWriterPlugin( OfxImageEffectHandle handle );

public:
	JpegWriterProcessParams getProcessParams(const OfxTime time);
	virtual void render( const OFX::RenderArguments& args );

public:
	OFX::BooleanParam* _premult; ///< premult output by alpha
};

}
}
}
}

#endif
