#ifndef _PNG_WRITER_PLUGIN_HPP_
#define _PNG_WRITER_PLUGIN_HPP_

#include <tuttle/plugin/context/WriterPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace writer {

using namespace boost::gil;

struct PngWriterProcessParams
{
	std::string _filepath;      ///< filepath
	bool        _outputRGB;		///< Force RGB
	int         _bitDepth;      ///< Output bit depth
};

/**
 * @brief Png writer
 */
class PngWriterPlugin : public WriterPlugin
{
public:
	PngWriterPlugin( OfxImageEffectHandle handle );

public:
	PngWriterProcessParams getParams(const OfxTime time);
	virtual void render( const OFX::RenderArguments& args );

public:
	OFX::BooleanParam* _outputRGB;     ///< Force RGB
};

}
}
}
}

#endif
