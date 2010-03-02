#ifndef _PNG_WRITER_PLUGIN_HPP_
#define _PNG_WRITER_PLUGIN_HPP_

#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace writer {

using namespace boost::gil;

/**
 * @brief Png writer
 */
class PNGWriterPlugin : public OFX::ImageEffect
{
public:
	PNGWriterPlugin( OfxImageEffectHandle handle );
	OFX::Clip* getSrcClip() const;
	OFX::Clip* getDstClip() const;

public:
	virtual void render( const OFX::RenderArguments& args );
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

protected:
	OFX::PushButtonParam* _renderButton;  ///< Render push button
	OFX::StringParam* _filepath;      ///< Target file path
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _srcClip;       ///< Input image clip
	OFX::Clip* _dstClip;       ///< Ouput image clip
	bool _bRenderOnce;   ///< Avoid multiple useless writing
};

}
}
}
}

#endif
