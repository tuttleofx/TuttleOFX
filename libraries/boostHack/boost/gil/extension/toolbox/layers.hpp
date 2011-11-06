// Copyright Tom Brinkman 2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef _layers_hpp_
#define _layers_hpp_

#include <boost/gil/gil_all.hpp>
#include <boost/function.hpp>

namespace layer
{

template <typename view_t>
struct layers
{
	typedef boost::function<void (view_t&)> layer_t;	
	std::vector<layer_t> lst;	

	layers(layer_t* p, int n)
	{
		for (int m = 0; m < n; ++m)
			lst.push_back(p[m]);
	}
	
	void operator()(view_t& view)
	{
		for (std::size_t n = 0; n < lst.size(); ++n)
			lst[n](view);
	}
};

}

#endif
