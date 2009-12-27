/**
 * @file EXRWriterPlugin.hpp
 * @brief
 * @author
 * @date    16/12/09 15:34
 *
 */

#ifndef EXRWRITER_PLUGIN_H
#define EXRWRITER_PLUGIN_H

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

/**
 * @brief
 *
 */
class EXRWriterPlugin : public OFX::ImageEffect
{
public:
	EXRWriterPlugin( OfxImageEffectHandle handle );
	OFX::Clip* getSrcClip() const;
	OFX::Clip* getDstClip() const;

public:
	virtual void render( const OFX::RenderArguments& args );
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

protected:
	OFX::PushButtonParam* _renderButton;  ///< Render push button
	OFX::StringParam*     _filepath;      ///< Target file path
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _srcClip;       ///< Source image clip
	OFX::Clip* _dstClip;       ///< Ouput image clip
	bool _bRenderOnce;         ///< Avoid multiple useless writing

};

}
}
}
}

#endif  // EXRWRITER_PLUGIN_H
