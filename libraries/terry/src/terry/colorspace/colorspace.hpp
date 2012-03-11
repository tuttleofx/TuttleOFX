#ifndef _TERRY_COLOR_COLORSPACE_HPP_
#define	_TERRY_COLOR_COLORSPACE_HPP_

#include "colorspace/xyz.hpp"

//#include "layout.hpp"
//#include "gradation.hpp"
//#include "primaries.hpp"
//#include "temperature.hpp"

#include <tuttle/common/utils/global.hpp>

#include <terry/basic_colors.hpp>
#include <terry/numeric/init.hpp>

#include <terry/numeric/operations.hpp>

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/pop_back.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/mpl/zip_view.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/print.hpp>
#include <boost/type_traits/is_same.hpp>

#include <boost/ptr_container/ptr_vector.hpp>

#include <boost/numeric/ublas/matrix.hpp>

#include <boost/fusion/mpl.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/adapted/mpl.hpp>
#include <boost/fusion/sequence/intrinsic.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>

#include <boost/integer/static_min_max.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_base_of.hpp>

#include <vector>
#include <typeinfo>
#include <iostream>


namespace terry {

using namespace ::boost::gil;

namespace color {

/**
ll lib	 * @brief Allows to retrieve an mpl::vector of all color types hierachy.
 * @example for HSL: from_root=[XYZ,RGB,HSL], to_root=[HSL,RGB,XYZ]
 */
template<class color>
struct color_dependencies
{
	/// list of color types dependencies from root (eg. HSL: [XYZ,RGB,HSL])
	typedef typename ::boost::mpl::push_back<typename color_dependencies<typename color::reference>::from_root, color>::type from_root;
	/// list of color types dependencies to root (eg. HSL: [HSL,RGB,XYZ])
	typedef typename ::boost::mpl::push_front<typename color_dependencies<typename color::reference>::to_root, color>::type to_root;
	///< number of color types dependencies (eg. HSL: 3)
	typedef typename ::boost::mpl::size<from_root>::type size;

	struct details
	{
		typedef typename ::boost::mpl::pop_back<from_root>::type from_root_A;
		typedef typename ::boost::mpl::pop_front<from_root>::type from_root_B;
		typedef typename ::boost::mpl::pop_back<to_root>::type to_root_A;
		typedef typename ::boost::mpl::pop_front<to_root>::type to_root_B;
	};
	/// a vector of colors pairs (eg. HSL: [(XYZ,RGB),(RGB,HSL)])
	typedef typename ::boost::mpl::zip_view< ::boost::mpl::vector<typename details::from_root_A, typename details::from_root_B> >::type color_steps_from_root;
	/// a vector of colors pairs (eg. HSL: [(HSL,RGB),(RGB,XYZ)])
	typedef typename ::boost::mpl::zip_view< ::boost::mpl::vector<typename details::to_root_A, typename details::to_root_B> >::type color_steps_to_root;
};
template<>
struct color_dependencies<IsRootReference>
{
	typedef ::boost::mpl::vector<> from_root;
	typedef ::boost::mpl::vector<> to_root;
};

/**
 * @brief Mpl functor to convert a list of color types into a list of color params types
 * @example HSL>RGB>XYZ to HSLParams>RGBParams>XYZParams
 */
template< typename Color >
struct color_to_colorparams_mplfunc
{
	typedef typename Color::params type;
};

/**
 * @brief All needed parameters to fully defined a specific colorspace.
 * 
 * To fully defined a colorspace we need to defined parameters all the color
 * hierarchy, because each colorspace is defined relatively to another. Finally,
 * they are all defined directly or indirectly to XYZ.
 * 
 * @example HSL colorspace hierachy is [XYZ, RGB, HSL] and XYZ has no parameter,
 * so you need to set RGBParams and HSLParams.
 */
template<class Color>
struct FullColorParams
{
	typedef FullColorParams<Color> This;
	
	typedef color_dependencies<Color> dependencies;
	typedef typename dependencies::from_root from_root;
	typedef typename dependencies::to_root to_root;
	typedef typename dependencies::size size;
	
	typedef typename ::boost::mpl::transform<from_root, color_to_colorparams_mplfunc< ::boost::mpl::_1 > >::type full_params; ///< all params from root to Color
	typedef typename ::boost::fusion::result_of::as_vector<full_params>::type full_params_v;
	full_params_v _params;
	
	/**
	 * @brief Color hierarchy size.
	 */
	std::size_t getNbReferences() const
	{
		return size::value;
	}
};

template< typename Color, typename ChannelType >
struct color_to_pixel
{
	typedef boost::gil::pixel<ChannelType, typename Color::layout> type;
};

/**
 * @brief Utility class to compute all internal types needed by color_transformation function.
 * 
 * @param ColorsFromRoot mpl vector of colors corresponding to the steps to go
 *                         from one color to the root XYZ color.
 * @param sizeLeft size of the remaining parts of the color steps to process
 * @param ChannelType type used from pixels (defined the bitdepth used: bits8, bits32f, ...)
 * @param fromRoot direction of the color conversion
 */
template< typename ColorsFromRoot, std::size_t sizeLeft, class ChannelType, bool fromRoot >
struct color_transformation_types_impl
{
	static const std::size_t size = ::boost::mpl::size<ColorsFromRoot>::value;
	static const std::size_t indexMax = size-1; // only to get valid typedefs to compile...
	static const std::size_t indexDirFromRoot = size - sizeLeft; // direction from root, index from root
	static const std::size_t indexDirToRoot = sizeLeft - 1; // direction to root, index from root
	
	// with sizeLeft = 2
	//
	// [ XYZ,  RGB, CMYK, PLOP, PLIP ] : colors_from_color created from PLIP colorspace
	//
	//    | > > | > > | > > ^     |    : direction fromRoot
	//    0     1     2     3     |    :   index fromRoot => indexDirFromRoot
	//    |     ^ < < | < < | < < |    : direction toRoot
	//    |     3     2     1     0    :   index toRoot
	//    0     1     |     |     |    :   index fromRoot ==> indexDirToRoot
	static const std::size_t sIndex = fromRoot ? indexDirFromRoot : indexDirToRoot;
	static const std::size_t dIndex = ::boost::static_unsigned_min<fromRoot ? sIndex+1 : sIndex-1, indexMax>::value; // incrementation depends from the direction
	static const std::size_t nextIndex = ::boost::static_unsigned_min<fromRoot ? dIndex+1 : dIndex-1, indexMax>::value;
	
	static const std::size_t colorParamIndex = fromRoot ? sIndex+1 : sIndex;
	static const std::size_t nextColorParamIndex = fromRoot ? dIndex+1 : dIndex;
		
	typedef typename ::boost::mpl::at_c<ColorsFromRoot, sIndex>::type SColor; // current color index
	typedef typename ::boost::mpl::at_c<ColorsFromRoot, dIndex>::type DColor; // dest color index (destination for current color_transformation_step)
	typedef typename ::boost::mpl::at_c<ColorsFromRoot, nextIndex>::type NextColor; // next color index
	typedef typename ::boost::mpl::back<ColorsFromRoot>::type Color; // color from which the ColorsFromRoot mpl::vector is build
	typedef typename ::boost::mpl::if_c<fromRoot,
		typename ::boost::mpl::back<ColorsFromRoot>::type,
		typename ::boost::mpl::front<ColorsFromRoot>::type
		>::type FColor; // final destination color
	
	typedef FullColorParams<Color> FullParams;
	typedef boost::gil::pixel<ChannelType, typename SColor::layout> SPixel;
	typedef boost::gil::pixel<ChannelType, typename DColor::layout> DPixel;
	typedef boost::gil::pixel<ChannelType, typename NextColor::layout> NextPixel;
	typedef boost::gil::pixel<ChannelType, typename FColor::layout> FPixel;
};

/**
 */
template<class ColorsFromRoot, std::size_t sizeLeft, class ChannelType, bool fromRoot>
struct color_transformation_static_for_impl_rec
{
	typedef color_transformation_types_impl<ColorsFromRoot, sizeLeft, ChannelType, fromRoot> T;
	
	void operator()(
			const typename T::FullParams& params,
			const typename T::SPixel& src,
			      typename T::DPixel& dst,
			      typename T::FPixel& finalDst
		) const
	{
		BOOST_STATIC_ASSERT( sizeLeft > 3 );
		
		color_transformation_step(
			boost::fusion::at_c<T::index>( params._params ),
			src,
			dst
			);

		typename T::NextPixel pixNext;

		color_transformation_static_for_impl_rec<ColorsFromRoot, sizeLeft-1, ChannelType, fromRoot>()(
			params,
			dst,
			pixNext,
			finalDst
			);
	}
};

/**
 */
template<class ColorsFromRoot, class ChannelType, bool fromRoot>
struct color_transformation_static_for_impl_rec<ColorsFromRoot, 3, ChannelType, fromRoot>
{
	static const std::size_t sizeLeft = 3;
	typedef color_transformation_types_impl<ColorsFromRoot, sizeLeft, ChannelType, fromRoot> T;
	
	void operator()(
			const typename T::FullParams& params,
			const typename T::SPixel& src,
			      typename T::DPixel& dst,
			      typename T::FPixel& finalDst
		) const
	{
		// 2 steps to finish !
		
		color_transformation_step(
			boost::fusion::at_c<T::colorParamIndex>( params._params ), // A
			src,
			dst
			);

		color_transformation_step(
			boost::fusion::at_c<T::nextColorParamIndex>( params._params ), // B
			dst,
			finalDst
			);
	}
};

/**
 */
template<class ColorsFromRoot, std::size_t colorsSize, class ChannelType, bool fromRoot>
struct color_transformation_static_for_impl
{
	typedef color_transformation_types_impl<ColorsFromRoot, colorsSize, ChannelType, fromRoot> T;

	BOOST_STATIC_ASSERT( colorsSize == boost::mpl::size<ColorsFromRoot>::value );
	BOOST_STATIC_ASSERT( colorsSize > 0 );
	
	void operator()(
			const typename T::FullParams& params,
			const typename T::SPixel& src,
			      typename T::FPixel& dst
		) const
	{
		typename T::DPixel pixNext;
		
		// multiple steps in a static foreach color_transformation_step
		color_transformation_static_for_impl_rec<ColorsFromRoot, colorsSize, ChannelType, fromRoot>()(
			params,
			src,
			pixNext,
			dst
			);
	}
};
// specialized for a size left == 2
template<class ColorsFromRoot, class ChannelType, bool fromRoot>
struct color_transformation_static_for_impl<ColorsFromRoot, 2, ChannelType, fromRoot>
{
	static const std::size_t sizeLeft = 2;
	typedef color_transformation_types_impl<ColorsFromRoot, sizeLeft, ChannelType, fromRoot> T;
	
	void operator()(
			const typename T::FullParams& params,
			const typename T::SPixel& src,
			      typename T::FPixel& dst
		) const
	{
		// only one step
		color_transformation_step(
			boost::fusion::at_c<T::colorParamIndex>( params._params ),
			src,
			dst
			);
	}
};
template<class ColorsFromRoot, class ChannelType, bool fromRoot>
struct color_transformation_static_for_impl<ColorsFromRoot, 1, ChannelType, fromRoot>
{
	static const std::size_t colorsSize = 1;
	typedef color_transformation_types_impl<ColorsFromRoot, colorsSize, ChannelType, fromRoot> T;
	
	void operator()(
			const typename T::FullParams& params,
			const typename T::SPixel& src,
			      typename T::FPixel& dst
		) const
	{
		dst = src;
	}
};

/**
 * @brief Intern function of color_transformation, which statically iterate
 *        over the colorspace dependencies.
 */
template<class ColorsFromRoot, class ChannelType, bool fromRoot>
struct color_transformation_static_for
{
	static const std::size_t sizeLeft = ::boost::mpl::size<ColorsFromRoot>::value;
	typedef color_transformation_types_impl<ColorsFromRoot, sizeLeft, ChannelType, fromRoot> T;
	BOOST_STATIC_ASSERT( sizeLeft > 0 );
	
	void operator()(
			const typename T::FullParams& params,
			const typename T::SPixel& src,
			      typename T::FPixel& dst
		) const
	{
		color_transformation_static_for_impl<ColorsFromRoot, sizeLeft, ChannelType, fromRoot>()(
			params,
			src,
			dst
			);
	}
};

/**
 */
template<class Color, class ChannelType>
void color_transformation_to_root(
		const FullColorParams<Color>& params,
		const boost::gil::pixel<ChannelType, typename Color::layout>& src,
		      boost::gil::pixel<ChannelType,XYZ::layout>& dst )
{
	color_transformation_static_for<typename FullColorParams<Color>::from_root, ChannelType, false>()(
		params,
		src,
		dst
		);
}

/**
 */
template<class Color, typename ChannelType>
void color_transformation_from_root(
		const boost::gil::pixel<ChannelType,XYZ::layout>& src,
		const FullColorParams<Color>& params,
		      boost::gil::pixel<ChannelType, typename Color::layout>& dst
	)
{
	color_transformation_static_for<typename FullColorParams<Color>::from_root, ChannelType, true>()(
		params,
		src,
		dst
		);
}

/**
 * @brief To convert a pixel from a colorspace to another.
 */
template<class SColor, class DColor, typename ChannelType>
void color_transformation(
		const FullColorParams<SColor>& sParams,
		const boost::gil::pixel<ChannelType, typename SColor::layout>& src,
		const FullColorParams<DColor>& dParams,
		boost::gil::pixel<ChannelType, typename DColor::layout>& dst )
{
	using namespace terry;
	using namespace terry::numeric;
	
	boost::gil::pixel<ChannelType,XYZ::layout> root;

	color_transformation_to_root<SColor, ChannelType>( sParams, src, root );
	color_transformation_from_root<DColor, ChannelType>( root, dParams, dst );
}


/*
FullColorParams<cmyk> cmykParams;
cmykParams.get<rgb>().colorTemperature = d65;

color_transformation( cmykParams, pixCmyk, adobe_sRgb, pixRgb );
*/

}
}

#endif

