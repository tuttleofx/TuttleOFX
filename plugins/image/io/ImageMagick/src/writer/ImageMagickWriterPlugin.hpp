#ifndef _IMAGEMAGICK_WRITER_PLUGIN_HPP_
#define _IMAGEMAGICK_WRITER_PLUGIN_HPP_

#include <tuttle/plugin/context/WriterPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace imagemagick {
namespace writer {

using namespace boost::gil;

struct ImageMagickWriterProcessParams
{
	std::string _filepath;      ///< filepath
	bool        _premult;		///< Force RGB
	int         _bitDepth;      ///< Output bit depth
	int         _quality;
};

/**
 * @brief ImageMagick writer
 */
class ImageMagickWriterPlugin : public WriterPlugin
{
public:
	ImageMagickWriterPlugin( OfxImageEffectHandle handle );

public:
	ImageMagickWriterProcessParams getProcessParams(const OfxTime time);
	virtual void render( const OFX::RenderArguments& args );

public:
	OFX::BooleanParam* _premult; ///< premult output by alpha
	OFX::IntParam* _quality; ///< quality / compression for imagemagick
};

}
}
}
}

#endif
