#ifndef _TUTTLE_PLUGIN_EXRWRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_EXRWRITER_PLUGIN_HPP_

#include "EXRWriterDefinitions.hpp"
#include <tuttle/plugin/context/WriterPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

struct EXRWriterProcessParams
{
	std::string _filepath;       ///< filepath
	EParamBitDepth _bitDepth;        ///< Bit depth
	ECompType _componentsType;   ///< Components type
	bool _flip;
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
	void              render( const OFX::RenderArguments& args );

protected:
	OFX::ChoiceParam*     _componentsType;  ///< Components type
};

}
}
}
}

#endif
