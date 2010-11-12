#include "TextPlugin.hpp"
#include "TextProcess.hpp"
#include "TextDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace text {

using namespace boost::gil;

TextPlugin::TextPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );

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
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth dstBitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();

	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				TextProcess<rgba8_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				TextProcess<rgba16_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				TextProcess<rgba32f_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}
	else if( dstComponents == OFX::ePixelComponentAlpha )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				TextProcess<gray8_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				TextProcess<gray16_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				TextProcess<gray32f_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}
}

}
}
}
