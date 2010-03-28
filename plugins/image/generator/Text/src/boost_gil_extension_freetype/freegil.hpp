#ifndef _freegil_hpp_
#define _freegil_hpp_

// (C) Copyright Tom Brinkman 2007.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/cast.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>
#include <boost/spirit/include/classic_lists.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "utilgil.hpp"
#include "utilstl.hpp"

namespace boost {
namespace gil {

struct make_metric
{

	template <typename glyph_t >
		FT_Glyph_Metrics operator( )( const glyph_t& glyph )
	{
		BOOST_ASSERT( glyph.face );
		int load_flags = FT_LOAD_DEFAULT;
		int index = FT_Get_Char_Index( glyph.face, glyph.ch );
		FT_Load_Glyph( glyph.face, index, load_flags );
		return glyph.face->glyph->metrics;
	}
};

struct make_kerning
{
	int left_glyph;

	make_kerning( ) : left_glyph( 0 ) { }

	template <typename glyph_t>
		int operator( )( const glyph_t& glyph )
	{
		int right_glyph = FT_Get_Char_Index( glyph.face, glyph.ch );
		if( !FT_HAS_KERNING( glyph.face ) || !left_glyph || !right_glyph )
			return 0;

		FT_Vector delta;
		FT_Get_Kerning( glyph.face, left_glyph, right_glyph, FT_KERNING_DEFAULT, &delta );
		left_glyph = right_glyph;
		return delta.x >> 6;
	}
};

struct make_width
{
	int advance;
	int lastwidth;
	int lastadvance;

	make_width( ) : advance( 0 ), lastwidth( 0 ), lastadvance( 0 ) { }

	operator int( )
	{
		return advance - ( lastadvance - lastwidth );
	}

	void operator( )( FT_Glyph_Metrics metrics, int kerning)
	{
		lastadvance = kerning + ( metrics.horiAdvance >> 6 );
		lastwidth = ( metrics.width >> 6 );
		advance += lastadvance;
	}
};

struct make_advance_width
{
	int advance;

	make_advance_width( ) : advance( 0 ) { }

	operator int( )
	{
		return advance;
	}

	void operator( )( FT_Glyph_Metrics metrics, int kerning)
	{
		advance += kerning + ( metrics.horiAdvance >> 6 );
	}
};

struct make_advance_height
{
	int height;

	make_advance_height( ) : height( 0 ) { }

	operator int( )
	{
		return height;
	}

	void operator( )( FT_Glyph_Metrics metrics )
	{
		int advance = ( metrics.vertAdvance >> 6 );
		height = ( std::max )( height, advance );
	}
};

struct make_height
{
	int height;

	make_height( ) : height( 0 ) { }

	operator int( )
	{
		return height;
	}

	void operator( )( FT_Glyph_Metrics metrics )
	{
		int h = ( metrics.height >> 6 );
		height = ( std::max )( height, h );
	}
};

struct make_glyph_height
{
	int height;

	make_glyph_height( ) : height( 0 ) { }

	operator int( )
	{
		return height;
	}

	void operator( )( FT_Glyph_Metrics metrics )
	{
		int n = ( metrics.height >> 6 ) - ( metrics.horiBearingY >> 6 );
		height = ( std::max )( height, n );
	}
};

template <typename view_t>
class render_glyph
{
	typedef render_glyph<view_t> This;
	typedef typename view_t::value_type Pixel;
	const view_t& _view;
	const Pixel _color;
	const double _letterSpacing;
	int _x;
	
//	render_glyph( const This& );
	
public:
	render_glyph( const view_t & view, const Pixel& color, const double letterSpacing ) : _view( view ), _color(color), _letterSpacing(letterSpacing), _x( 0 ) { }

	template <typename glyph_t>
		void operator( )( const glyph_t& glyph, int kerning = 0 )
	{
		_x += kerning;

		FT_GlyphSlot slot = glyph.face->glyph;

		int load_flags = FT_LOAD_DEFAULT;
		int index = FT_Get_Char_Index( glyph.face, glyph.ch );
		FT_Load_Glyph( glyph.face, index, load_flags );
		FT_Render_Glyph( slot, FT_RENDER_MODE_NORMAL );

		int y = _view.height( ) - ( glyph.face->glyph->metrics.horiBearingY >> 6 );
		int width = glyph.face->glyph->metrics.width >> 6;
		int height = glyph.face->glyph->metrics.height >> 6;
		int xadvance = glyph.face->glyph->advance.x >> 6;

		BOOST_ASSERT( width == slot->bitmap.width );
		BOOST_ASSERT( height == slot->bitmap.rows );

		typedef gray8_pixel_t pixel_t;
		gray8c_view_t glyphview = interleaved_view( width, height, (pixel_t*) slot->bitmap.buffer, sizeof(unsigned char) * slot->bitmap.width );

		copy_and_convert_alpha_blended_pixels( color_converted_view<gray32f_pixel_t>( glyphview ), _color, subimage_view( _view, _x, y, width, height ) );

		_x += xadvance;
		_x += _letterSpacing;
	}
};

struct find_last_fitted_glyph
{
	int width, x;

	find_last_fitted_glyph( int width ) : width( width ), x( 0 ) { }

	bool operator( )( FT_Glyph_Metrics metric, int kerning)
	{
		x += kerning;
		int tmp = x + ( metric.width >> 6 );
		x += ( metric.horiAdvance >> 6 );
		return tmp > width;
	}
};

}
}

#endif


