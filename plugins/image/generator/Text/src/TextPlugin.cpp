#include "TextPlugin.hpp"
#include "TextProcess.hpp"
#include "TextDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace text {

TextPlugin::TextPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_text          = fetchStringParam( kText );
	_font          = fetchStringParam( kFont );
	_size          = fetchIntParam( kSize );
	_ratio         = fetchDoubleParam( kRatio );
	_color         = fetchRGBAParam( kColor );
	_position      = fetchDouble2DParam( kPosition );
	_letterSpacing = fetchDoubleParam( kLetterSpacing );
	_verticalFlip  = fetchBooleanParam( kVerticalFlip );
}

TextProcessParams TextPlugin::getProcessParams() const
{
	TextProcessParams params;

	params._text          = _text->getValue();
	params._font          = _font->getValue();
	params._fontY         = _size->getValue();
	params._fontX         = params._fontY * _ratio->getValue();
	params._fontColor     = _color->getValue();
	params._position      = ofxToGil( _position->getValue() );
	params._letterSpacing = _letterSpacing->getValue();
	params._verticalFlip  = _verticalFlip->getValue();

	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void TextPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<TextProcess>( *this, args );
}

}
}
}
