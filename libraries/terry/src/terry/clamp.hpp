#ifndef _TERRY_CLAMP_HPP_
#define _TERRY_CLAMP_HPP_

namespace terry {


template <typename SChannel, typename DChannel>
struct channel_clamp
{
	const SChannel _min;
	const SChannel _max;
	
	channel_clamp( const SChannel min, const SChannel max )
	: _min(min)
	, _max(max)
	{}

	GIL_FORCEINLINE
	void operator()(const SChannel& src, DChannel& dst) const
	{
		typedef typename channel_traits<SChannel>::value_type src_value_t;
		typedef typename channel_traits<DChannel>::value_type dst_value_t;
		
		dst = dst_value_t( std::min( src_value_t( channel_traits<DChannel>::max_value() )
							, std::max( src_value_t( channel_traits<DChannel>::min_value() ) , src )
							)
		);
	}
};

template<typename SPixel, typename DPixel = SPixel>
struct default_clamp_converter
{
	typedef typename boost::gil::channel_type<SPixel>::type SChannel;
	typedef typename boost::gil::channel_type<DPixel>::type DChannel;
	
	const SChannel _min;
	const SChannel _max;
	
	default_clamp_converter()
	: _min( boost::gil::channel_traits<SChannel>::min_value() )
	, _max( boost::gil::channel_traits<SChannel>::max_value() )
	{}

	default_clamp_converter( const SChannel min, const SChannel max )
	: _min(min)
	, _max(max)
	{}

	GIL_FORCEINLINE
	void operator()( const SPixel& src, DPixel& dst ) const
	{
		using namespace boost::gil;
		static_for_each(src,dst,channel_clamp<SChannel, DChannel>(_min,_max));
	}
};

// from image_view_factory.hpp
// the difference with color_convert is on same pixels, only return pixel without process it.
namespace detail {

	template <typename SrcConstRefP, typename DstP, typename CC=default_clamp_converter<DstP> >        // const_reference to the source pixel and destination pixel value
	class clamp_convert_deref_fn : public deref_base<clamp_convert_deref_fn<SrcConstRefP,DstP,CC>, DstP, DstP, const DstP&, SrcConstRefP, DstP, false> {
	private:
		CC _cc; // clamp-converter
	public:
		clamp_convert_deref_fn() {}
		clamp_convert_deref_fn(CC cc_in) : _cc(cc_in) {}
	
		DstP operator()(SrcConstRefP srcP) const {
			DstP dstP;
			_cc(srcP,dstP);
			return dstP;
		}
	};


	// Add clamp converter upon dereferencing
	template <typename SrcView, typename CC, typename DstP, typename SrcP>
	struct _clamp_converted_view_type {
	private:
		typedef clamp_convert_deref_fn<typename SrcView::const_t::reference,DstP,CC> deref_t;
		typedef typename SrcView::template add_deref<deref_t> add_ref_t;
	public:
		typedef typename add_ref_t::type type;
		static type make(const SrcView& sv,CC cc) {return add_ref_t::make(sv,deref_t(cc));}
	};
	
	template <typename SrcView, typename DstP, typename CC=default_clamp_converter<DstP> >
	struct clamp_converted_view_type : public detail::_clamp_converted_view_type<SrcView,
	                                                                             CC,
	                                                                             DstP,
	                                                                             typename SrcView::value_type> {
		GIL_CLASS_REQUIRE(DstP, boost::gil, MutablePixelConcept)//why does it have to be mutable???
	};
	
	template <typename DstP, typename View, typename CC>
	inline typename clamp_converted_view_type<View,DstP,CC>::type clamp_converted_view(const View& src,CC cc) {
		return clamp_converted_view_type<View,DstP,CC>::make(src,cc);
	}
	
	template <typename DstP, typename View>
	inline typename clamp_converted_view_type<View,DstP>::type
	clamp_converted_view(const View& src) {
		return clamp_converted_view<DstP>(src,default_clamp_converter<DstP>());
	}
}

template <typename View>
GIL_FORCEINLINE
typename detail::clamp_converted_view_type<View, typename View::value_type, default_clamp_converter<typename View::value_type> >::type clamp_view( const View& sView )
{
	using namespace boost::gil;
	typedef typename View::value_type Pixel;

	return detail::clamp_converted_view<Pixel>( sView, default_clamp_converter<Pixel>() );
}

}

#endif

