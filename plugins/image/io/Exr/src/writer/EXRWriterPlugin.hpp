#ifndef EXRWRITER_PLUGIN_H
#define EXRWRITER_PLUGIN_H

#include <tuttle/plugin/context/WriterPlugin.hpp>
#include "EXRWriterDefinitions.hpp"

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

struct EXRWriterProcessParams
{
	std::string _filepath;       ///< filepath
	EParamBitDepth _bitDepth;        ///< Bit depth
	ECompType _componentsType;   ///< Components type
};

/**
 * @brief
 *
 */
class EXRWriterPlugin : public WriterPlugin
{
public:
	EXRWriterPlugin( OfxImageEffectHandle handle );

public:
	EXRWriterProcessParams getProcessParams( const OfxTime time );
	void                   render( const OFX::RenderArguments& args );

protected:
	OFX::ChoiceParam*     _componentsType;  ///< Components type
};

}
}
}
}

#endif  // EXRWRITER_PLUGIN_H
