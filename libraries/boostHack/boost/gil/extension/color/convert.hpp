#ifndef _BOOST_GIL_COLOR_CONVERT_HPP_
#define	_BOOST_GIL_COLOR_CONVERT_HPP_

#include <boost/gil/color_convert.hpp>

namespace boost { namespace gil {

template <typename DstP, typename SrcP>
inline
DstP color_convert(const SrcP& src)
{
	DstP dst;
    default_color_converter()(src,dst);
	return dst;
}


}
}

#endif

