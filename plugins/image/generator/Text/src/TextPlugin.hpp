#ifndef _TUTTLE_PLUGIN_TEXT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_TEXT_PLUGIN_HPP_

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace text {

struct TextProcessParams
{
	std::string _text;
};

/**
 * @brief Text plugin
 */
class TextPlugin : public OFX::ImageEffect
{
public:
    TextPlugin( OfxImageEffectHandle handle );

public:
    void render( const OFX::RenderArguments &args );
	
	TextProcessParams getProcessParams() const;
	
public:
    // do not need to delete these, the ImageEffect is managing them for us
    OFX::Clip* _srcClip; ///< Source image clip
    OFX::Clip* _dstClip; ///< Destination image clip
	OFX::StringParam* _text; ///< the text to rasterize in the image
};

}
}
}

#endif
