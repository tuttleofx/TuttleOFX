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
	std::string             _filepath;          ///< filepath
	ETuttlePluginBitDepth   _bitDepth;          ///< Bit depth
	ETuttlePluginComponents _componentsType;    ///< Components type
	EParamStorage           _storageType;       ///< Storage type (Scan line or tile)
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
	OFX::ChoiceParam*     _storageType;     ///< Storage type
};

}
}
}
}

#endif
