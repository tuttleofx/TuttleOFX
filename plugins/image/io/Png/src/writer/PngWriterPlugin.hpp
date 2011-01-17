#ifndef _TUTTLE_PLUGIN_PNG_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_PNG_WRITER_PLUGIN_HPP_

#include "PngWriterDefinitions.hpp"
#include <tuttle/plugin/context/WriterPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace writer {

struct PngWriterProcessParams
{
	std::string _filepath;        ///< filepath
	EParamComponents _components; ///< output components
	int _bitDepth;                ///< Output bit depth
	bool _flip;
};

/**
 * @brief Png writer
 */
class PngWriterPlugin : public WriterPlugin
{
public:
	PngWriterPlugin( OfxImageEffectHandle handle );

public:
	PngWriterProcessParams getProcessParams( const OfxTime time );

	void                   render( const OFX::RenderArguments& args );

public:
	OFX::ChoiceParam* _paramOutputComponents;     ///< Choose components RGBA or RGB
};

}
}
}
}

#endif
