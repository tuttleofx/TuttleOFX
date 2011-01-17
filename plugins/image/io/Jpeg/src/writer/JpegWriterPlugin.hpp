#ifndef _TUTTLE_PLUGIN_JPEG_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_JPEG_WRITER_PLUGIN_HPP_

#include <tuttle/plugin/context/WriterPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace writer {

struct JpegWriterProcessParams
{
	std::string _filepath;      ///< filepath
	bool _premult;              ///< Premultiply by alpha or directly use RGB channels
	int _bitDepth;              ///< Output bit depth
	int _quality;
	bool _flip;
};

/**
 * @brief Jpeg writer
 */
class JpegWriterPlugin : public WriterPlugin
{
public:
	JpegWriterPlugin( OfxImageEffectHandle handle );

public:
	JpegWriterProcessParams getProcessParams( const OfxTime time );
	void               render( const OFX::RenderArguments& args );

public:
	OFX::BooleanParam* _paramPremult; ///< premult output by alpha
	OFX::IntParam* _paramQuality; ///< quality / compression for jpeg
};

}
}
}
}

#endif
