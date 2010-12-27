#ifndef _TUTTLE_PLUGIN_TEXT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_TEXT_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace text {

struct TextProcessParams
{
	std::string _text;
	std::string _font;
	int _fontX;
	int _fontY;
	OfxRGBAColourD _fontColor;
	boost::gil::point2<double> _position;
	double _letterSpacing;
	bool _verticalFlip;
};

/**
 * @brief Text plugin
 */
class TextPlugin : public ImageEffectGilPlugin
{
public:
	TextPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );

	TextProcessParams getProcessParams() const;

public:
	OFX::StringParam* _text; ///< the text to rasterize in the image
	OFX::StringParam* _font;
	OFX::IntParam* _size;
	OFX::DoubleParam* _ratio;
	OFX::RGBAParam* _color;
	OFX::Double2DParam* _position;
	OFX::DoubleParam* _letterSpacing;
	OFX::BooleanParam* _verticalFlip;
};

}
}
}

#endif
