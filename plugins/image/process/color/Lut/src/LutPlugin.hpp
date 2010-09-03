#ifndef _TUTTLE_PLUGIN_LUTPLUGIN_HPP_
#define _TUTTLE_PLUGIN_LUTPLUGIN_HPP_

#include "lutEngine/LutReader.hpp"
#include "lutEngine/Lut.hpp"

#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace lut {

/**
 * @brief
 *
 */
class LutPlugin : public OFX::ImageEffect
{
public:
	LutPlugin( OfxImageEffectHandle handle );
	OFX::Clip*       getSrcClip() const;
	OFX::Clip*       getDstClip() const;
	const LutReader& lutReader() const;
	LutReader&       lutReader();
	Lut3D&           lut3D();

public:
	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

protected:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _clipSrc;             ///< Source image clip
	OFX::Clip* _clipDst;             ///< Destination image clip
	OFX::StringParam* _sFilename;    ///< Filename
	LutReader _lutReader;               ///< Reader
	Lut3D _lut3D;
};

}
}
}

#endif
