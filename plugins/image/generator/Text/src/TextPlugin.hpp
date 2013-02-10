#ifndef _TUTTLE_PLUGIN_TEXT_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_TEXT_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include "TextDefinitions.hpp"

namespace tuttle {
namespace plugin {
namespace text {

struct TextProcessParams
{
	std::string _text;
	bool _isExpression;

	std::string _font;
        int _fontFamily;

	int _fontX;
	int _fontY;
	OfxRGBAColourD _fontColor;
	OfxRGBAColourD _backgroundColor;
	boost::gil::point2<double> _position;
	double _letterSpacing;
	EParamVAlign _vAlign;
	EParamHAlign _hAlign;
	bool _verticalFlip;
        bool _italic;
        bool _bold;
};

/**
 * @brief Text plugin
 */
class TextPlugin : public ImageEffectGilPlugin
{
public:
	TextPlugin( OfxImageEffectHandle handle );

public:
	TextProcessParams getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

	void render( const OFX::RenderArguments& args );

public:
	OFX::StringParam* _paramText; ///< the text to rasterize in the image
	OFX::BooleanParam* _paramIsExpression;

	OFX::StringParam* _paramFont;
        OFX::ChoiceParam * _paramFontFamily;

	OFX::IntParam* _paramSize;
	OFX::DoubleParam* _paramRatio;
	OFX::RGBAParam* _paramColor;
        OFX::RGBAParam* _paramBackgroundColor;
	OFX::Double2DParam* _paramPosition;
	OFX::DoubleParam* _paramLetterSpacing;
	OFX::ChoiceParam* _paramVAlign;
	OFX::ChoiceParam* _paramHAlign;
	OFX::BooleanParam* _paramVerticalFlip;
	OFX::BooleanParam* _paramItalic;
	OFX::BooleanParam* _paramBold;
};

}
}
}

#endif
