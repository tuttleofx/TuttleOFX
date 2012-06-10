#ifndef _TERRY_COLOR_CONVERTED_VIEW_HPP_
#define _TERRY_COLOR_CONVERTED_VIEW_HPP_

#include <terry/channel.hpp>

namespace terry {

// from image_view_factory.hpp
// the difference with color_convert is on same pixels, only return pixel without process it.
namespace detail {

template <typename SrcConstRefP, typename DstP, typename CC > // const_reference to the source pixel and destination pixel value
class color_process_deref_fn : public deref_base<color_process_deref_fn<SrcConstRefP, DstP, CC>, DstP, DstP, const DstP&, SrcConstRefP, DstP, false >
{
private:
	CC _cc; // color converter
	
public:
	color_process_deref_fn( ) { }

	color_process_deref_fn( const CC& cc_in ) : _cc( cc_in ) { }

	DstP operator( )( SrcConstRefP srcP ) const
	{
		DstP dstP;
		_cc( srcP, dstP );
		return dstP;
	}
};


// Add color converter upon dereferencing
template <typename CC, typename View>
struct _color_processed_view_type
{

private:
	typedef typename View::value_type Pixel;
	typedef color_process_deref_fn<typename View::const_t::reference, Pixel, CC> deref_t;
	typedef typename View::template add_deref<deref_t> add_ref_t;
public:
	typedef typename add_ref_t::type type;

	static type make( const View& sv, const CC& cc )
	{
		return add_ref_t::make( sv, deref_t( cc ) );
	}
};

template <typename CC, typename View>
struct color_processed_view_type : public detail::_color_processed_view_type<CC, View>
{
	GIL_CLASS_REQUIRE( typename View::value_type, boost::gil, MutablePixelConcept ) //why does it have to be mutable?
};

}

template<typename CC, typename View>
inline typename detail::color_processed_view_type<CC, View>::type color_processed_view( const View& src, const CC& cc )
{
	return detail::color_processed_view_type<CC, View>::make( src, cc );
}

}

#endif
