#ifndef _TUTTLE_PLUGIN_THINNING_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_THINNING_ALGORITHM_HPP_

#include <boost/gil/extension/channel.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>

namespace tuttle {
namespace plugin {
namespace thinning {

static const bool lutthin1[512] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, false, true, true, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, false, false, true, true, false, false, true, true, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, false, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, false, false, false, true, false, false, true, true, false, false, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, true, false, false, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true };
static const bool lutthin2[512] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, false, true, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, false, false, true, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, false, true, true, true, false, false, true, true, false, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, false, false, true, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, false, true, true, true, false, false, true, true, false, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, false, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, true, false, false, true, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, false, true, true, true, false, false, true, true, false, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, true, true, true, true, false, false, true, false, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true, true, false, true, true, true, false, false, true, true, false, true, true, true };

template<class SView, class DView=SView>
struct pixel_locator_thinning_t
{
	typedef typename SView::locator SLocator;
	typedef typename DView::locator DLocator;
	typedef typename SView::value_type SPixel;
	typedef typename DView::value_type DPixel;
	typedef typename SLocator::cached_location_t SCachedLocation;

	const bool* _lut;
	SPixel _white;
	SPixel _black;
	const SLocator _loc_ref;
	// LT CT RT
	// LC    RC
	// LB CB RB
	const SCachedLocation LT;
	const SCachedLocation CT;
	const SCachedLocation RT;
	const SCachedLocation LC;
	const SCachedLocation RC;
	const SCachedLocation LB;
	const SCachedLocation CB;
	const SCachedLocation RB;

	pixel_locator_thinning_t( const SView& src, const bool* lut )
	: _lut(lut)
	, _loc_ref(src.xy_at(0,0))
	, LT(_loc_ref.cache_location(-1,-1))
	, CT(_loc_ref.cache_location( 0,-1))
	, RT(_loc_ref.cache_location( 1,-1))

	, LC(_loc_ref.cache_location(-1, 0))
	, RC(_loc_ref.cache_location( 1, 0))

	, LB(_loc_ref.cache_location(-1, 1))
	, CB(_loc_ref.cache_location( 0, 1))
	, RB(_loc_ref.cache_location( 1, 1))
	{
		using namespace boost::gil;
		pixel_assigns_min( _black );
		pixel_assigns_max( _white );
	}

	DPixel operator()( const SLocator& src ) const
	{
		using namespace boost::gil;

		if( *src != _white )
		{
			return _black;
		}
		
		std::size_t id =  ( src[LT] == _white )       |
			             (( src[LC] == _white ) << 1) |
			             (( src[LB] == _white ) << 2) |
			             (( src[CT] == _white ) << 3) |
			             (( *src    == _white ) << 4) |
			             (( src[CB] == _white ) << 5) |
			             (( src[RT] == _white ) << 6) |
			             (( src[RC] == _white ) << 7) |
			             (( src[RB] == _white ) << 8);
		if( _lut[id] )
		{
			return _white;
		}
		return _black;
	}
};



}
}
}

#endif
