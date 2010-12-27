#ifndef _TUTTLE_PLUGIN_LUTPLUGIN_HPP_
#define _TUTTLE_PLUGIN_LUTPLUGIN_HPP_

#include "lutEngine/LutReader.hpp"
#include "lutEngine/Lut.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace lut {

/**
 * @brief
 *
 */
class LutPlugin : public ImageEffectGilPlugin
{
public:
	LutPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

public:
	OFX::StringParam* _sFilename;    ///< Filename

	LutReader _lutReader;               ///< Reader
	Lut3D _lut3D;
};

}
}
}

#endif
