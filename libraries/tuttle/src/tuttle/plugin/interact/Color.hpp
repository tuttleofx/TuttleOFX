#ifndef _TUTTLE_PLUGIN_INTERACT_COLOR_HPP_
#define _TUTTLE_PLUGIN_INTERACT_COLOR_HPP_

#include <ofxPixels.h>
#include <ofxCore.h>
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace interact {

struct IColor
{
	virtual ~IColor() = 0;
	virtual OfxRGBAColourD getColor( const OfxTime time ) const = 0;
};

struct Color : public IColor
{
	OfxRGBAColourD _color;

	Color( const OfxRGBAColourD& color )
	: _color(color)
	{}

	Color()
	{
		_color.r = _color.g = _color.b = _color.a = 1.0;
	}

	inline OfxRGBAColourD getColor( const OfxTime time ) const
	{
		return _color;
	}
};

struct ColorRGBAParam : public IColor
{
	const OFX::RGBAParam& _param;
	ColorRGBAParam( const OFX::RGBAParam* param )
		: _param( *param )
	{}

	inline OfxRGBAColourD getColor( const OfxTime time ) const
	{
		return _param.getValue();
	}
};

struct ColorRGBParam : public IColor
{
	const OFX::RGBParam& _param;
	ColorRGBParam( const OFX::RGBParam* param )
		: _param( *param )
	{}

	inline OfxRGBAColourD getColor( const OfxTime time ) const
	{
		OfxRGBAColourD color;
		color.a = 1.0;
		_param.getValue( color.r, color.g, color.b );
		return color;
	}
};


}
}
}

#endif

