// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef _fill_hpp_
#define _fill_hpp_

#include <boost/function.hpp>
#include <boost/gil/gil_all.hpp>

namespace layer
{

template <typename view_t>
struct fill
{
	typedef typename view_t::value_type value_type_t;
	value_type_t color;
	fill(value_type_t color) : color(color) {}

	void operator()(view_t& view)
	{
		boost::gil::fill_pixels(view,color);
	}
};

template <typename view_t>
struct fill_horizontal
{
	typedef boost::function<void (view_t&)> funct_t;
	double value1;
	double value2;
	double minvalue;
	double maxvalue;
	funct_t funct;
	fill_horizontal(funct_t funct, double value1, double value2, double minvalue, double maxvalue) :
		funct(funct), minvalue(minvalue), maxvalue(maxvalue), value1(value1), value2(value2) {}

	void operator()(view_t& view)
	{
		using namespace boost::gil;

		double diff = maxvalue-minvalue;
		if (diff == 0)
			return;

		double rval = 1-((value1-minvalue)/(diff));
		int y1 = (int)(rval*(view.height()-1));

		rval = 1-((value2-minvalue)/(diff));
		int y2 = (int)(rval*(view.height()-1));

		int yt = (std::min)(y1,y2);
		int yb = (std::max)(y1,y2);

		view_t v2 = subimage_view(view,0,yt,view.width(),yb-yt);
		funct(v2);
	}
};

}

#endif
