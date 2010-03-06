/**
 * @file LutPlugin.hpp
 * @brief
 * @author
 * @date    01/10/09 12:01
 *
 */

#ifndef LUT_PLUGIN_H
#define LUT_PLUGIN_H

#include <ofxsImageEffect.h>
#include "lutEngine/lut_reader.h"
#include "lutEngine/hd3d_lut.h"

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
	OFX::Clip* getSrcClip() const;
	OFX::Clip* getDstClip() const;
	const LutReader & lutReader() const;
	LutReader & lutReader();
    Lut3D & lut3D();
public:
	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

protected:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _srcClip;             ///< Source image clip
	OFX::Clip* _dstClip;             ///< Destination image clip
	OFX::StringParam* _sFilename;    ///< Filename
	LutReader _lutReader;               ///< Reader
    Lut3D     _lut3D;
};

}
}
}

#endif  // LUT_PLUGIN_H
