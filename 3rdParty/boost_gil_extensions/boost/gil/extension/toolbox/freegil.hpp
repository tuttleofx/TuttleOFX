// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef _freegil_hpp_
#define _freegil_hpp_

#include <boost/gil/gil_all.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H

#include <blend.hpp>

namespace layer
{

struct glyph
{
	glyph(char ch, FT_Face face, boost::gil::rgb8_pixel_t pixel) :
		ch(ch), face(face), pixel(pixel){}

	char ch;
	FT_Face face;
	boost::gil::rgb8_pixel_t pixel;
};

struct make_metric
{
	FT_Glyph_Metrics operator()(glyph& glyph)
	{
		BOOST_ASSERT(glyph.face);
		int load_flags = FT_LOAD_DEFAULT;
		int index = FT_Get_Char_Index(glyph.face,glyph.ch);
		FT_Load_Glyph(glyph.face, index, load_flags);
		return glyph.face->glyph->metrics;
	}
};

struct make_width
{
	int advance;
	int lasbwidth;
	int lastadvance;
	make_width() : advance(0), lasbwidth(0), lastadvance(0) {}
	operator int(){return advance-(lastadvance-lasbwidth);}
	void operator()(FT_Glyph_Metrics metrics)
	{
		lastadvance = metrics.horiAdvance >> 6; 
		lasbwidth = (metrics.width >> 6);
		advance += lastadvance;
	}
};

struct make_advance_width
{
	int advance;
	make_advance_width() : advance(0){}
	operator int(){return advance;}
	void operator()(FT_Glyph_Metrics metrics)
	{
		advance += metrics.horiAdvance >> 6;
	}
};

struct make_advance_height
{
	int height;
	make_advance_height() : height(0){}
	operator int(){return height;}
	void operator()(FT_Glyph_Metrics metrics)
	{
		int advance = (metrics.vertAdvance >> 6);
		height = (std::max)(height,advance);
	}
};

struct make_height
{
	int height;
	make_height() : height(0){}
	operator int(){return height;}
	void operator()(FT_Glyph_Metrics metrics)
	{
		int h = (metrics.height >> 6);
		height = (std::max)(height,h);
	}
};

struct make_glyph_height
{
	int height;
	make_glyph_height() : height(0) {} 
	operator int(){return height;}
	void operator()(FT_Glyph_Metrics metrics)
	{
		int n = (metrics.height >> 6) -(metrics.horiBearingY >> 6);
		height = (std::max)(height,n);
	}
};

struct find_last_fitted_glyph
{
	int width,x;
	find_last_fitted_glyph(int width) : width(width),x(0){}
	bool operator()(FT_Glyph_Metrics metric)
	{
		int tmp = x + (metric.width >> 6);
		x += (metric.horiAdvance >> 6); 
		return tmp > width;
	}
};

template <typename view_t>
struct render_glyphs
{
	int x;
	const view_t& view;
	render_glyphs(const view_t& view) : view(view), x(0) {}	

	void operator()(glyph& glyph)
	{
		using namespace boost::gil;
		
		FT_GlyphSlot glyphslot = glyph.face->glyph; 
		FT_Face face = glyph.face;			

		int load_flags = FT_LOAD_DEFAULT;
		int index = FT_Get_Char_Index(face,glyph.ch);
		FT_Load_Glyph(face, index, load_flags);
		FT_Render_Glyph(glyphslot, FT_RENDER_MODE_NORMAL);

		int y = view.height() - (face->glyph->metrics.horiBearingY >> 6);
		int width = face->glyph->metrics.width >> 6;
		int height = face->glyph->metrics.height >> 6;
		int xadvance = face->glyph->advance.x >> 6;

		BOOST_ASSERT(view.width());
		BOOST_ASSERT(width == glyphslot->bitmap.width);
		BOOST_ASSERT(height == glyphslot->bitmap.rows);

		gray8c_view_t gray = interleaved_view(width,height,
			(gray8_pixel_t*)glyphslot->bitmap.buffer,glyphslot->bitmap.width);

		copy<alpha24_blend>(glyph.pixel,gray,subimage_view(view,x,y,width,height));
		x += xadvance; 
	}
};

static inline FT_Error face_requester(FTC_FaceID id, FT_Library library, void* data, FT_Face* face)
{
	char** paths = (char**)data;
	char* path = paths[(long)id]; 
	return FT_New_Face(library, path, 0, face);
}

template <typename view_t>
struct text
{
	enum 
	{ 
		left = (0x1 << 0),
		center = (0x1 << 1),
		right = (0x1 << 2),
		top = (0x1 << 3),
		ftop = (0x1 << 4),
		middle = (0x1 << 5),  //when text is aligned adjacent to each other.
		fmiddle = (0x1 << 6), //when the text is all by itself
		bottom = (0x1 << 7),
	}; 

	enum
	{
		fill = (0x1 << 0),
		clipspecial = (0x1 << 1),
		fillexcess =  (0x1 << 2),
	};

	typedef typename view_t::value_type color_t;

	FTC_Manager manager;
	std::string str;
	int id;
	int size;
	int align;
	int options;
	char special;
	color_t color;
	std::vector<glyph> glyphs;

	text(FTC_Manager manager, std::string str, color_t color, 
		int id, int size, int align = center|middle, int options = 0, char special = '#') : 
			manager(manager), str(str), id(id), size(size), align(align), 
				color(color), options(options), special(special){}

	text(FTC_Manager manager, color_t color, 
		int id, int size, int align = center|middle, int options = 0, char special = '.') : 
			manager(manager), id(id), size(size), align(align), 
				color(color), options(options), special(special){}

	void operator()(view_t& view)
	{
		FTC_ScalerRec_ scaler;
		scaler.face_id = (FTC_FaceID)id;
		scaler.width = size;
		scaler.height = size;
		scaler.pixel = 1;
		scaler.x_res = 0;
		scaler.y_res = 0;
		
		FT_Size asize;
		FT_Error error = FTC_Manager_LookupSize(manager, &scaler, &asize);
		if (error)
			return;
		
		for (int n = 0; n < str.size(); ++n)
			glyphs.push_back(glyph(str[n],asize->face,color));

		std::vector<FT_Glyph_Metrics> m;			
		std::transform(glyphs.begin(),glyphs.end(), std::back_inserter(m), make_metric());		

		int mwidth = std::for_each(m.begin(), m.end(), make_width());

		if (options & fill || (options & fillexcess && mwidth > view.width()))
		{
			glyphs.clear();
			for (int n = 0; n < 500; ++n)
				glyphs.push_back(glyph(special,asize->face,color));

			m.clear();			
			std::transform(glyphs.begin(),glyphs.end(),
				std::back_inserter(m), make_metric());
		}

		if (options & clipspecial)
		{
			int pos = -1;
			for (pos = 0; pos < glyphs.size(); ++pos)
				if (glyphs[pos].ch == special)
					break;
		
			if (pos >= 1 && pos < glyphs.size())
			{
				std::vector<glyph> front,back;
				std::copy(glyphs.begin(), glyphs.begin()+pos, std::back_inserter(front));
				std::copy(glyphs.begin()+pos, glyphs.end(), std::back_inserter(back));
				glyphs.clear();
		
				while (true)
				{
					std::vector<glyph> glyphs2;
					std::copy(front.begin(),front.end(), std::back_inserter(glyphs2));	
					std::copy(back.begin(),back.end(), std::back_inserter(glyphs2));	
		
					std::vector<FT_Glyph_Metrics> metrics;		
					std::transform(glyphs2.begin(),glyphs2.end(),
						std::back_inserter(metrics), make_metric());
		
					std::vector<FT_Glyph_Metrics>::iterator it = 
						std::find_if(metrics.begin(),metrics.end(),
							find_last_fitted_glyph(view.width()));

					if (it != metrics.end())
						front.pop_back();
					else
						break;
				}
		
				std::copy(front.begin(), front.end(), std::back_inserter(glyphs));
				std::copy(back.begin(), back.end(), std::back_inserter(glyphs));
			}
		}

		m.clear();		
		std::transform(glyphs.begin(),glyphs.end(), std::back_inserter(m), make_metric());

		std::vector<FT_Glyph_Metrics>::iterator it = 
			std::find_if(m.begin(), m.end(), find_last_fitted_glyph(view.width()));
		int distance = (int)std::distance(it,m.end());
		glyphs.erase(glyphs.end()-distance,glyphs.end());
		
		m.clear();		
		std::transform(glyphs.begin(),glyphs.end(), std::back_inserter(m), make_metric());

		mwidth = std::for_each(m.begin(), m.end(), make_width());
		int mheight = std::for_each(m.begin(), m.end(), make_height());
		int gheight = std::for_each(m.begin(), m.end(), make_glyph_height());

		int x = 0;
		if (align & center)
			x = (view.width()-mwidth)/2;
		else if (align & right)
			x = view.width()-mwidth;
	
		int y = 0;
		if (align & middle)
			y = (view.height()-mheight-gheight)/2;
		else if (align & fmiddle)
			y = (view.height()-mheight)/2;
		else if (align & bottom)
			y = view.height()-mheight;

		BOOST_ASSERT(!glyphs.empty());
		view = subimage_view(view,x,y,mwidth,mheight);
		std::for_each(glyphs.begin(), glyphs.end(), render_glyphs<view_t>(view));
	}
};

template <typename view_t>
struct elipsed_layer
{
	typedef typename view_t::value_type color_t;

	FTC_Manager manager;
	std::string sfront;
	std::string sback;
	int id;
	int size;
	color_t color;
	static const int dwidth = 30;

	elipsed_layer(FTC_Manager manager, std::string sfront, std::string sback, color_t color, int id, int size) : 
			manager(manager), sfront(sfront), sback(sback), id(id), size(size), color(color){}

	void operator()(view_t& view)
	{
		typedef text<view_t> text_t;

		text_t lback(manager,sback,color,id,size,
			text_t::right|text_t::bottom);
		view_t vback = view;
		lback(vback);
		
		text_t lfront(manager,sfront,color,id,size,
			text_t::left|text_t::bottom);
		int width = view.width()-vback.width()-dwidth;
		view_t vfront = subimage_view(view,0,0,width,view.height());
		lfront(vfront);

		text_t ldots(manager,color,id,size,	
			text_t::center|text_t::bottom,text_t::fill,'.');
		width = view.width()-vfront.width()-vback.width();
		view_t vdots = subimage_view(view,vfront.width(),0,width,view.height());		
		ldots(vdots);
	}	
};

}

#endif
