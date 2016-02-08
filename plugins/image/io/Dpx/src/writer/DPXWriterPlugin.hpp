#ifndef _TUTTLE_PLUGIN_DPXWRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_DPXWRITER_PLUGIN_HPP_

#include <tuttle/ioplugin/context/WriterPlugin.hpp>
#include "DPXWriterDefinitions.hpp"

#include <libdpx/DPX.h>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

struct DPXWriterProcessParams
{
	std::string               _filepath;       ///< filepath
	std::string               _project;        ///< project metadata
	std::string               _copyright;      ///< copyright metadata
	ETuttlePluginBitDepth     _bitDepth;       ///< Output bit depth
	size_t                    _iBitDepth;      ///< Int value of output bit depth
	::dpx::Descriptor         _descriptor;     ///< Components type
	::dpx::Characteristic     _transfer;
	::dpx::Characteristic     _colorimetric;
	::dpx::Packing            _packed;         ///< Bit streaming packing
	::dpx::Encoding           _encoding;
	::dpx::Orientation        _orientation;
	bool                      _swapEndian;     ///< set endianness

};

/**
 * @brief
 *
 */
class DPXWriterPlugin : public WriterPlugin
{
public:
	DPXWriterPlugin( OfxImageEffectHandle handle );

public:
	DPXWriterProcessParams getProcessParams( const OfxTime time );

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	void render( const OFX::RenderArguments& args );

protected:
	OFX::ChoiceParam*    _bitDepth;        ///< Dpx bit depth
	OFX::ChoiceParam*    _descriptor;      ///< Dpx descriptor
	OFX::ChoiceParam*    _transfer;        ///< Dpx transfer
	OFX::ChoiceParam*    _colorimetric;    ///< Dpx colorimetric
	OFX::ChoiceParam*    _packed;          ///< Dpx packed method
	OFX::BooleanParam*   _swapEndian;      ///< Dpx swap endian
	OFX::ChoiceParam*    _encoding;        ///< Dpx encoding
	OFX::ChoiceParam*    _orientation;     ///< Dpx orientation
	OFX::StringParam*    _project;         ///< Dpx metadata Project
	OFX::StringParam*    _copyright;       ///< Dpx metadata Copyright
};

}
}
}
}

#endif
