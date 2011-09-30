// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef _border_hpp_
#define _border_hpp_

#include <boost/gil/gil_all.hpp>

namespace layer
{

template <typename view_t>
struct border
{
	typedef typename view_t::value_type color_t;
	color_t clr;
	
	border(color_t clr) : clr(clr) {}

	void operator()(view_t& view)
	{
		using namespace boost::gil;		
		
		for (int x = 0; x < view.width(); ++x)
		{
			view(x,0) = clr;
			view(x,view.height()-1) = clr;
		}

		for (int y = 0; y < view.height(); ++y)
		{
			view(0,y) = clr;
			view(view.width()-1,y) = clr;
		}
	}
};	

template <typename view_t>
struct raised_border
{
	typedef typename view_t::value_type color_t;
	color_t dark,light;
	
	raised_border(color_t light, color_t dark) : dark(dark), light(light) {}

	void operator()(view_t& view)
	{
		using namespace boost::gil;		
		
		for (int x = 0; x < view.width(); ++x)
			view(x,0) = light;

		for (int y = 0; y < view.height(); ++y)
			view(0,y) = light;	

		for (int y = 1; y < view.height(); ++y)
			view(view.width()-1,y) = dark;	

		for (int x = 0; x < view.width(); ++x)
			view(x,view.height()-1) = dark;	
	}
};	

}

#endif
