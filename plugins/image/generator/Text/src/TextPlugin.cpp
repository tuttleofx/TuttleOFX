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
	_paramText          = fetchStringParam( kParamText );
	_paramIsExpression  = fetchBooleanParam( kParamIsExpression );
	_paramFont          = fetchStringParam( kParamFont );
	_paramSize          = fetchIntParam( kParamSize );
	_paramRatio         = fetchDoubleParam( kParamRatio );
	_paramColor         = fetchRGBAParam( kParamColor );
	_paramPosition      = fetchDouble2DParam( kParamPosition );
	_paramLetterSpacing = fetchDoubleParam( kParamLetterSpacing );
	_paramVAlign        = fetchChoiceParam( kParamVAlign );
	_paramHAlign        = fetchChoiceParam( kParamHAlign );
	_paramVerticalFlip  = fetchBooleanParam( kParamVerticalFlip );
}

TextProcessParams TextPlugin::getProcessParams() const
{
	TextProcessParams params;

	params._text          = _paramText->getValue();
	params._isExpression  = _paramIsExpression->getValue();
	params._font          = _paramFont->getValue();
	params._fontY         = _paramSize->getValue();
	params._fontX         = params._fontY * _paramRatio->getValue();
	params._fontColor     = _paramColor->getValue();
	params._position      = ofxToGil( _paramPosition->getValue() );
	params._letterSpacing = _paramLetterSpacing->getValue();
	params._vAlign        = static_cast<EParamVAlign>( _paramVAlign->getValue() );
	params._hAlign        = static_cast<EParamHAlign>( _paramHAlign->getValue() );
	params._verticalFlip  = _paramVerticalFlip->getValue();

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
