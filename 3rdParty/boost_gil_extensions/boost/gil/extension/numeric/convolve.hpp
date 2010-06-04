/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
*/

/*************************************************************************************************/

#ifndef GIL_CONVOLVE_HPP
#define GIL_CONVOLVE_HPP

/*!
/// \file 
/// \brief 2D seperable convolutions and correlations
///
/// \author Hailin Jin and Lubomir Bourdev \n
///         Adobe Systems Incorporated
/// \date   2005-2007 \n Last updated on February 6, 2007
*/


#include <cstddef>
#include <cassert>
#include <algorithm>
#include <vector>
#include <functional>
#include <boost/gil/gil_config.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/metafunctions.hpp>
#include "pixel_numeric_operations.hpp"
#include "algorithm.hpp"

namespace boost { namespace gil {

/// \ingroup ImageAlgorithms
/// Boundary options for 1D correlations/convolutions
enum convolve_boundary_option  {
    convolve_option_output_ignore,   /// do nothing to the output
    convolve_option_output_zero,     /// set the output to zero
    convolve_option_extend_padded,   /// assume the source boundaries to be padded already
    convolve_option_extend_zero,     /// assume the source boundaries to be zero
    convolve_option_extend_constant, /// assume the source boundaries to be the boundary value
    convolve_option_extend_mirror    /// assume the source boundaries to be the mirror of source
};

namespace detail {

/// \ingroup PointModel
template <typename T> GIL_FORCEINLINE
bool operator>=(const point2<T>& p1, const point2<T>& p2) { return (p1.x>=p2.x && p1.y>=p2.y); }
/// \ingroup PointModel
template <typename T> GIL_FORCEINLINE
bool operator<=(const point2<T>& p1, const point2<T>& p2) { return (p1.x<=p2.x && p1.y<=p2.y); }
/// \ingroup PointModel
template <typename T, typename T2> GIL_FORCEINLINE
bool operator>=(const point2<T>& p, const T2 v) { return (p.x>=v && p.y>=v); }
/// \ingroup PointModel
template <typename T, typename T2> GIL_FORCEINLINE
bool operator<=(const point2<T>& p, const T2 v) { return (p.x<=v && p.y<=v); }

/// compute the correlation of 1D kernel with the rows of an image
/// \param src source view
/// \param dst destination view
/// \param ker dynamic size kernel
/// \param dst_tl topleft point of dst in src coordinates. Must be Point(0,0) if src.dimensions()==dst.dimensions().
///        We can see it as a vector to move dst in src coordinates.
/// \param option boundary option
/// \param correlator correlator functor
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView,typename Correlator>
void correlate_rows_imp(const SrcView& src, const Kernel& ker, const DstView& dst, const typename SrcView::point_t& dst_tl,
                        convolve_boundary_option option,
                        Correlator correlator) {
	// assert dst frame with shift is inside src frame
    assert(src.dimensions()>=dst.dimensions());
	// dst must be contained in src
    assert(dst_tl >= 0);
    assert(dst_tl <= src.dimensions());
	assert(dst_tl + dst.dimensions() <= src.dimensions());
    assert(ker.size()!=0);

	typedef typename SrcView::point_t point_t;
	typedef typename point_t::value_type coord_t;
    typedef typename pixel_proxy<typename SrcView::value_type>::type PIXEL_SRC_REF;
    typedef typename pixel_proxy<typename DstView::value_type>::type PIXEL_DST_REF;
    typedef typename Kernel::value_type kernel_type;

    if(ker.size()==1) {//reduces to a multiplication
		view_multiplies_scalar<PixelAccum>( subimage_view(src, dst_tl, dst.dimensions()),
			                                *ker.begin(),
			                                dst );
        return;
    }

    if (dst.dimensions().x==0 || dst.dimensions().y==0)
		return;
	
	//  ................................................................
	//  .                     src with kernel size adds                .
	//  .                                                              .
	//  .          _________________________________________           .
	//  .          |             src and dst               |           .
	//  .          |          ____________                 |           .
	// <. left_out | left_in |            | right_in       | right_out .>
	//  .          |         |    roi     |                |           .
	//  .          |         |            |                |           .
	//  .          |         |____________|                |           .
	//  .          |_______________________________________|           .
	//  .                                                              .
	//  ................................................................
	// < > : represents the temporary buffer
	const point_t dst_br  = dst_tl + dst.dimensions();
	const coord_t left_in   = std::min(static_cast<coord_t>(ker.left_size()), dst_tl.x);
	const coord_t left_out  = std::max(static_cast<coord_t>(ker.left_size()) - dst_tl.x, (coord_t)0);
	const coord_t right_tmp = src.dimensions().x - dst_br.x;
	const coord_t right_in  = std::min(static_cast<coord_t>(ker.right_size()), right_tmp);
	const coord_t right_out = std::max(static_cast<coord_t>(ker.right_size()) - right_tmp, (coord_t)0);

	COUT_VAR2( src.dimensions().x, src.dimensions().y );
	COUT_VAR2( dst.dimensions().x, dst.dimensions().y );
	COUT_VAR2( dst_tl.x, dst_tl.y );
	COUT_VAR2( dst_br.x, dst_br.y );
	COUT_VAR( left_in );
	COUT_VAR( left_out );
	COUT_VAR( right_tmp );
	COUT_VAR( right_in );
	COUT_VAR( right_out );

    PixelAccum acc_zero; pixel_zeros_t<PixelAccum>()(acc_zero);

	if (option==convolve_option_output_ignore || option==convolve_option_output_zero)
	{
        typename DstView::value_type dst_zero; pixel_assigns_t<PixelAccum,PIXEL_DST_REF>()(acc_zero,dst_zero);
        if (dst.dimensions().x<static_cast<coord_t>(ker.size()))
		{
            if (option==convolve_option_output_zero)
                fill_pixels(dst,dst_zero);
        }
		else
		{
			std::vector<PixelAccum> buffer(dst.dimensions().x+left_in+right_in);
            for(coord_t yy=0;yy<dst.dimensions().y;++yy)
			{
				coord_t yy_src = yy + dst_tl.y;
                assign_pixels(src.x_at(dst_tl.x-left_in,yy_src), src.x_at(dst_br.x+right_in,yy_src), &buffer.front());

                typename DstView::x_iterator it_dst=dst.row_begin(yy);
                if (option==convolve_option_output_zero)
                    it_dst = std::fill_n(it_dst,left_out,dst_zero);

				int buffer_dst_size = buffer.size()-left_out-right_out;
				correlator( &buffer.front(), &buffer.front()+buffer_dst_size, // why not always use begin(), does front() have a performance impact ?
                            ker.begin(), it_dst );
                it_dst += buffer_dst_size;

                if (option==convolve_option_output_zero)
                    std::fill_n(it_dst,right_out,dst_zero);
                //it_dst += right_out;
            }
        }
    }
	else
	{
        std::vector<PixelAccum> buffer( dst.dimensions().x + (ker.size() - 1) );
		const coord_t srcRoi_left = dst_tl.x - left_in;
		const coord_t srcRoi_right = dst_br.x - right_in;
		const coord_t srcRoi_width = dst.dimensions().x + left_in + right_in;
        for(int yy=0; yy<dst.dimensions().y; ++yy)
		{
			coord_t yy_src = yy + dst_tl.y;
			// fill buffer from src view depending on boundary option
            switch( option )
			{
				case convolve_option_extend_padded:
				{
					PixelAccum* it_buffer=&buffer.front();
					assign_pixels( src.x_at(dst_tl.x-ker.left_size(),yy_src),
								   src.x_at(dst_br.x+ker.right_size(),yy_src),
								   it_buffer );
					break;
				}
				case convolve_option_extend_zero:
				{
					PixelAccum* it_buffer=&buffer.front();
					it_buffer = std::fill_n(it_buffer,left_out,acc_zero);

					it_buffer = assign_pixels(src.x_at(srcRoi_left,yy_src),src.x_at(srcRoi_right,yy_src),it_buffer);

					std::fill_n(it_buffer,right_out,acc_zero);
					break;
				}
				case convolve_option_extend_constant:
				{
					PixelAccum* it_buffer=&buffer.front();
					PixelAccum filler;
					pixel_assigns_t<PIXEL_SRC_REF,PixelAccum>()(*src.x_at(srcRoi_left,yy_src),filler);
					it_buffer = std::fill_n(it_buffer,left_out,filler);

					it_buffer = assign_pixels(src.x_at(srcRoi_left,yy_src),src.x_at(srcRoi_right,yy_src),it_buffer);

					pixel_assigns_t<PIXEL_SRC_REF,PixelAccum>()(*src.x_at(srcRoi_right-1,yy_src),filler);
					std::fill_n(it_buffer,right_out,filler);
					break;
				}
				case convolve_option_extend_mirror:
				{
					PixelAccum* it_buffer = &buffer.front();
					typedef typename SrcView::reverse_iterator reverse_iterator;
					const unsigned int nleft = static_cast<unsigned int>(left_out / srcRoi_width);
					coord_t copy_size = buffer.size();
					const coord_t left_rest = left_out % srcRoi_width;
					bool reverse;
					if( nleft % 2 ) // odd
					{
						assign_pixels( src.at(srcRoi_right-1-left_rest,yy_src),
									   src.at(srcRoi_right-1,yy_src),
									   it_buffer );
						reverse = true; // next step reversed
					}
					else
					{
						assign_pixels( reverse_iterator(src.at(srcRoi_left+left_rest,yy_src)),
									   reverse_iterator(src.at(srcRoi_left,yy_src)),
									   it_buffer );
						reverse = false; // next step not reversed
					}
					it_buffer += left_rest;
					copy_size -= left_rest;
					while( copy_size )
					{
						coord_t tmp_size;
						if( copy_size > srcRoi_width ) // if kernel left size > src width... (extrem case)
							tmp_size = srcRoi_width;
						else // standard case
							tmp_size = copy_size;
						
						if( reverse )
						{
							assign_pixels( reverse_iterator(src.at(srcRoi_right,yy_src)),
										   reverse_iterator(src.at(srcRoi_right-tmp_size,yy_src)),
										   it_buffer );
						}
						else
						{
							assign_pixels( src.at(srcRoi_left,yy_src),
										   src.at(srcRoi_left+tmp_size,yy_src),
										   it_buffer );
						}
						it_buffer += tmp_size;
						copy_size -= tmp_size;
						reverse = !reverse;
					}
					break;
				}
				case convolve_option_output_ignore:
				case convolve_option_output_zero:
					assert(false);
            }
            correlator( &buffer.front(),&buffer.front()+dst.dimensions().x,
                        ker.begin(),
                        dst.row_begin(yy) );
        }
    }
}

template <typename PixelAccum>
class correlator_n {
private:
    std::size_t _size;
public:
    correlator_n(std::size_t size_in) : _size(size_in) {}
    template <typename SrcIterator,typename KernelIterator,typename DstIterator>
    void operator()(SrcIterator src_begin,SrcIterator src_end,
                    KernelIterator ker_begin,
                    DstIterator dst_begin) {
        correlate_pixels_n<PixelAccum>(src_begin,src_end,ker_begin,_size,dst_begin);
    }
};
template <std::size_t Size,typename PixelAccum>
struct correlator_k {
public:
    template <typename SrcIterator,typename KernelIterator,typename DstIterator>
    void operator()(SrcIterator src_begin,SrcIterator src_end,
                    KernelIterator ker_begin,
                    DstIterator dst_begin){
        correlate_pixels_k<Size,PixelAccum>(src_begin,src_end,ker_begin,dst_begin);
    }
};
} // namespace detail

/// \ingroup ImageAlgorithms
///correlate a 1D variable-size kernel along the rows of an image
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
GIL_FORCEINLINE
void correlate_rows(const SrcView& src, const Kernel& ker, const DstView& dst, const typename SrcView::point_t& dst_tl,
                    convolve_boundary_option option=convolve_option_extend_zero) {
    detail::correlate_rows_imp<PixelAccum>(src,ker,dst,dst_tl,option,detail::correlator_n<PixelAccum>(ker.size()));
}

/// \ingroup ImageAlgorithms
///correlate a 1D variable-size kernel along the rows of an image
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
GIL_FORCEINLINE
void correlate_rows(const SrcView& src, const Kernel& ker, const DstView& dst,
                    convolve_boundary_option option=convolve_option_extend_zero) {
	typename SrcView::point_t zero(0,0);
    detail::correlate_rows_imp<PixelAccum>(src,ker,dst,zero,option,detail::correlator_n<PixelAccum>(ker.size()));
}

/// \ingroup ImageAlgorithms
///correlate a 1D variable-size kernel along the columns of an image
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
GIL_FORCEINLINE
void correlate_cols(const SrcView& src, const Kernel& ker, const DstView& dst, const typename SrcView::point_t& dst_tl,
                    convolve_boundary_option option=convolve_option_extend_zero) {
    correlate_rows<PixelAccum>(transposed_view(src),ker,transposed_view(dst),typename SrcView::point_t(dst_tl.y, dst_tl.x),option);
}

/// \ingroup ImageAlgorithms
///correlate a 1D variable-size kernel along the columns of an image
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
GIL_FORCEINLINE
void correlate_cols(const SrcView& src, const Kernel& ker, const DstView& dst,
                    convolve_boundary_option option=convolve_option_extend_zero) {
    correlate_rows<PixelAccum>(transposed_view(src),ker,transposed_view(dst),option);
}

/// \ingroup ImageAlgorithms
///convolve a 1D variable-size kernel along the rows of an image
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
GIL_FORCEINLINE
void convolve_rows(const SrcView& src, const Kernel& ker, const DstView& dst, const typename SrcView::point_t& dst_tl,
                   convolve_boundary_option option=convolve_option_extend_zero) {
    correlate_rows<PixelAccum>(src,reverse_kernel(ker),dst,dst_tl,option);
}

/// \ingroup ImageAlgorithms
///convolve a 1D variable-size kernel along the rows of an image
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
GIL_FORCEINLINE
void convolve_rows(const SrcView& src, const Kernel& ker, const DstView& dst,
                   convolve_boundary_option option=convolve_option_extend_zero) {
    correlate_rows<PixelAccum>(src,reverse_kernel(ker),dst,option);
}

/// \ingroup ImageAlgorithms
///convolve a 1D variable-size kernel along the columns of an image
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
GIL_FORCEINLINE
void convolve_cols(const SrcView& src, const Kernel& ker, const DstView& dst, const typename SrcView::point_t& dst_tl,
                   convolve_boundary_option option=convolve_option_extend_zero) {
    convolve_rows<PixelAccum>(transposed_view(src),ker,transposed_view(dst),typename SrcView::point_t(dst_tl.y, dst_tl.x),option);
}

/// \ingroup ImageAlgorithms
///convolve a 1D variable-size kernel along the columns of an image
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
GIL_FORCEINLINE
void convolve_cols(const SrcView& src, const Kernel& ker, const DstView& dst,
                   convolve_boundary_option option=convolve_option_extend_zero) {
    convolve_rows<PixelAccum>(transposed_view(src),ker,transposed_view(dst),option);
}

/// \ingroup ImageAlgorithms
///correlate a 1D fixed-size kernel along the rows of an image
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
GIL_FORCEINLINE
void correlate_rows_fixed(const SrcView& src, const Kernel& ker, const DstView& dst,
                          convolve_boundary_option option=convolve_option_extend_zero) {
	typename SrcView::point_t topleft(0,0);
    detail::correlate_rows_imp<PixelAccum>(src,ker,dst,topleft,option,detail::correlator_k<Kernel::static_size,PixelAccum>());
}

/// \ingroup ImageAlgorithms
///correlate a 1D fixed-size kernel along the columns of an image
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
GIL_FORCEINLINE
void correlate_cols_fixed(const SrcView& src, const Kernel& ker, const DstView& dst,
                          convolve_boundary_option option=convolve_option_extend_zero) {
    correlate_rows_fixed<PixelAccum>(transposed_view(src),ker,transposed_view(dst),option);
}

/// \ingroup ImageAlgorithms
///convolve a 1D fixed-size kernel along the rows of an image
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
GIL_FORCEINLINE
void convolve_rows_fixed(const SrcView& src, const Kernel& ker, const DstView& dst,
                         convolve_boundary_option option=convolve_option_extend_zero) {
    correlate_rows_fixed<PixelAccum>(src,reverse_kernel(ker),dst,option);
}

/// \ingroup ImageAlgorithms
///convolve a 1D fixed-size kernel along the columns of an image
template <typename PixelAccum,typename SrcView,typename Kernel,typename DstView>
GIL_FORCEINLINE
void convolve_cols_fixed(const SrcView& src, const Kernel& ker, const DstView& dst,
                         convolve_boundary_option option=convolve_option_extend_zero) {
    convolve_rows_fixed<PixelAccum>(transposed_view(src),ker,transposed_view(dst),option);
}

} }  // namespace boost::gil

#endif
