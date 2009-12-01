/*
    Copyright 2005-2007 Adobe Systems Incorporated

    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).

    See http://opensource.adobe.com/gil for most recent version including documentation.
*/
/*************************************************************************************************/

#ifndef GIL_COLOR_MERGE_HPP
#define GIL_COLOR_MERGE_HPP

////////////////////////////////////////////////////////////////////////////////////////
/// \file
/// \brief GIL default color merging
/// \author Eloi Du Bois
/// \date   October 27, 2009
///
///
////////////////////////////////////////////////////////////////////////////////////////

#include <functional>
#include <boost/gil/gil_config.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/channel_algorithm.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/metafunctions.hpp>
#include <boost/gil/utilities.hpp>

namespace boost { namespace gil {

////////////////////////////////////////////////////////////////////////////////////////
///
///                 COLOR MERGING
///
////////////////////////////////////////////////////////////////////////////////////////

/// @defgroup ColorMerge Color Space Merging
/// \ingroup ColorSpaces
/// \brief Support for conversion between pixels of different color spaces and channel depths

/// \ingroup ChannelMergeAlgorithm
/// \brief A binary_function function object converting between channel types
template <template <typename> class Functor,
          typename SrcChannelAV,typename SrcChannelBV,typename DstChannelV> // Model ChannelValueConcept
struct channel_merging : public std::binary_function<SrcChannelAV,SrcChannelBV,DstChannelV> {
    DstChannelV
    operator()(SrcChannelAV srcA, SrcChannelBV srcB) const {
        return Functor<DstChannelV>::merge(srcA, srcB);
    }
    template <typename ItA,typename ItB>
    DstChannelV
    operator()(const ItA &itA,const ItA &itB,SrcChannelAV srcA, SrcChannelBV srcB) const {
        return Functor<DstChannelV>::merge(get_color(itA, alpha_t()), get_color(itB, alpha_t()), srcA, srcB);
    }
};

/// \ingroup ChannelMergeAlgorithm
/// \brief Merge from two channels type to another.
template <typename ItA,typename ItB,template <typename> class Functor,
          typename DstChannel,typename SrcChannelA,typename SrcChannelB>
inline typename channel_traits<DstChannel>::value_type
channel_merge(const ItA &itA,const ItB &itB,SrcChannelA &srcA,SrcChannelB &srcB)
{
    return channel_merging<
               Functor,
               typename channel_traits<SrcChannelA>::value_type,
               typename channel_traits<SrcChannelB>::value_type,
               typename channel_traits<DstChannel >::value_type>()(
                    itA, itB,
                    channel_convert<DstChannel>(srcA),
                    channel_convert<DstChannel>(srcB));
}

/// \ingroup ChannelMergeAlgorithm
/// \brief Merge from two channels type to another.
template <template <typename> class Functor,
          typename DstChannel,typename SrcChannelA,typename SrcChannelB>
inline typename channel_traits<DstChannel>::value_type
channel_merge(SrcChannelA &srcA,SrcChannelB &srcB)
{
    return channel_merging<
               Functor,
               typename channel_traits<SrcChannelA>::value_type,
               typename channel_traits<SrcChannelB>::value_type,
               typename channel_traits<DstChannel >::value_type>()(
                    channel_convert<DstChannel>(srcA),
                    channel_convert<DstChannel>(srcB));
}

/// \ingroup ChannelConvertAlgorithm
/// \brief Same as channel_converter, except it takes the destination channel 
///        by reference, which allows us to move the templates from the class
///        level to the method level. This is important when invoking it on
///        heterogeneous pixels.
template<template <typename> class Functor,typename ItA,typename ItB>
class default_alpha_channel_merging {
    protected:
        // View iterators are needed because merging depends on each alpha
        // channel
        const ItA & _itA;   /// View iterator src A
        const ItB & _itB;   /// View iterator src B
    public:
        default_alpha_channel_merging(const ItA & itA, const ItB & itB)
        : _itA(itA)
        , _itB(itB) { }

        template <typename Ch1,typename Ch2,typename Ch3>
        void operator()(const Ch1& srcA,const Ch2& srcB,Ch3& dst) const {
            dst=channel_merge<Functor, Ch3,ItA,ItB>(_itA,_itB,srcA,srcB);
        }

        template <typename Ch1,typename Ch2,typename Ch3>
        Ch3 operator()(const Ch1& srcA,const Ch2& srcB) const {
            return channel_merge<Functor, Ch3>(_itA,_itB,srcA,srcB);
        }
};

template<template <typename> class Functor>
struct default_channel_merging {
    template <typename Ch1,typename Ch2,typename Ch3>
    void operator()(const Ch1& srcA,const Ch2& srcB,Ch3& dst) const {
        dst=channel_merge<Functor, Ch3>(srcA,srcB);
    }

    template <typename Ch1,typename Ch2,typename Ch3>
    Ch3 operator()(const Ch1& srcA,const Ch2& srcB) const {
        return channel_merge<Functor, Ch3>(srcA,srcB);
    }
};

/// \ingroup ColorMerging
/// \brief Color merging object.
template <template <typename> class Functor>
struct default_color_merging_impl {
    template<typename ItA, typename ItB, typename P1, typename P2, typename P3>
    void operator()(const ItA& itA,const ItB& itB,
                    const P1& srcA,const P2& srcB,
                    P3& dst) const {
        static_for_each(srcA,srcB,dst,
                        default_alpha_channel_merging<Functor,ItA,ItB>(itA,itB));
    }

    template<typename P1, typename P2, typename P3>
    void operator()(const P1& srcA,const P2& srcB,P3& dst) const {
        static_for_each(srcA,srcB,dst,default_channel_merging<Functor>());
    }
};

/// @defgroup ColorMerge Color Space merging
/// \ingroup ColorMerge
/// \brief Support for merge between pixels of different color spaces and channel depths

/// \ingroup ColorMerge
/// \brief class for color-converting one pixel to another
template<template <typename> class Functor>
struct default_color_merging {
    template <typename ItA,typename ItB,typename SrcAP,typename SrcBP,typename DstP>
    void operator()(const ItA &itA,const ItB &itB,
                    const SrcAP& srcA,const SrcBP& srcB,
                    DstP& dst) const {
        default_color_merging_impl<Functor>()(itA,itB,srcA,srcB,dst);
    }
    template <typename SrcAP, typename SrcBP, typename DstP>
    void operator()(const SrcAP& srcA,const SrcBP& srcB,DstP& dst) const {
        default_color_merging_impl<Functor>()(srcA,srcB,dst);
    }
};

/// \ingroup ColorMerge
/// \brief helper function for merge two pixels to another using GIL default color-merging
///     where ScrAP models HomogeneousPixelConcept
///           ScrBP models HomogeneousPixelConcept
///           DstP models HomogeneousPixelValueConcept
template <template <typename> class Functor, typename SrcAP, typename SrcBP, typename DstP>
inline void color_merge(const SrcAP& srcA, const SrcBP& srcB, DstP& dst) {
    default_color_merging<Functor>()(srcA,srcB,dst);
}

/// \brief merge_pixels with two sources
template <typename View1, typename View2, typename View3, typename F> GIL_FORCEINLINE
F merge_pixels(const View1& src1, const View2& src2,const View3& dst, F fun) {
    for (std::ptrdiff_t y=0; y<dst.height(); ++y) {
        typename View1::x_iterator srcIt1=src1.row_begin(y);
        typename View2::x_iterator srcIt2=src2.row_begin(y);
        typename View3::x_iterator dstIt=dst.row_begin(y);
        for (std::ptrdiff_t x=0; x<dst.width(); ++x)
            // Compile time resolved
//            if ( mpl::contains<typename color_space_type<Pixel>::type,alpha_t>() == mpl::true_ )
                fun(srcIt1[x],srcIt2[x],dstIt[x]);
    }
    return fun;
}

/// \brief merge_pixels with two sources
template <typename View1, typename View2, typename View3, typename F> GIL_FORCEINLINE
F alpha_merge_pixels(const View1& src1, const View2& src2,const View3& dst, F fun) {
    for (std::ptrdiff_t y=0; y<dst.height(); ++y) {
        typename View1::x_iterator srcIt1=src1.row_begin(y);
        typename View2::x_iterator srcIt2=src2.row_begin(y);
        typename View3::x_iterator dstIt=dst.row_begin(y);
        for (std::ptrdiff_t x=0; x<dst.width(); ++x)
            fun(srcIt1,srcIt2,srcIt1[x],srcIt2[x],dstIt[x]);
    }
    return fun;
}

} }  // namespace boost::gil

#endif
