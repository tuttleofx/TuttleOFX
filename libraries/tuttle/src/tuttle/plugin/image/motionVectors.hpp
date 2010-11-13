#ifndef _TUTTLE_PLUGIN_IMAGE_MOTIONVECTORS_HPP_
#define _TUTTLE_PLUGIN_IMAGE_MOTIONVECTORS_HPP_

#include <tuttle/plugin/IProgress.hpp>
#include <boost/gil/extension/channel.hpp>

#include <boost/gil/utilities.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>

#include <boost/mpl/if.hpp>
#include <boost/type_traits/is_same.hpp>

#include <cmath>

namespace tuttle {
namespace plugin {

namespace bgil = boost::gil;

/**
 * @brief change intensity and rotate vectors.
 * @param [in/out] xVecView image of x vectors
 * @param [in/out] yVecView image of y vectors
 * @param [in] angle rotation to apply on vectors (radian)
 * @param [in] intensity scale vectors values
 * @param [in] p inform progress
 */
template< typename View>
// Models RandomAccess2DImageViewConcept
bool modifyVectors( const View& xVecView, const View& yVecView,
                    const double angle, const double intensity,
                    tuttle::plugin::IProgress* p )
{
	BOOST_ASSERT( yVecView.width() != 0 );
	BOOST_ASSERT( yVecView.height() != 0 );
	BOOST_ASSERT( yVecView.width() == xVecView.width() );
	BOOST_ASSERT( yVecView.height() == xVecView.height() );

	typedef typename View::point_t Point2Integer;
	typedef typename boost::gil::channel_base_type<typename boost::gil::channel_type<View>::type>::type VecChannel;
	typedef typename boost::gil::point2<VecChannel> VecPoint2;

	const double cosAngle = std::cos( angle );
	const double sinAngle = std::sin( angle );

	for( int y = 0;
	     y < xVecView.height();
	     ++y )
	{
		typename View::x_iterator it_xVec    = xVecView.row_begin( y );
		typename View::x_iterator itEnd_xVec = xVecView.row_end( y );
		typename View::x_iterator it_yVec    = yVecView.row_begin( y );
		for( ;
		     it_xVec != itEnd_xVec;
		     ++it_xVec, ++it_yVec )
		{
			VecPoint2 gradient;
			gradient.x = bgil::get_color( *it_xVec, bgil::gray_color_t() );
			gradient.y = bgil::get_color( *it_yVec, bgil::gray_color_t() );

			// apply rotation on gradient vector
			VecPoint2 motion;
			motion.x = gradient.x * cosAngle + gradient.y * sinAngle;
			motion.y = gradient.y * cosAngle - gradient.x * sinAngle;

			motion *= intensity;

			bgil::get_color( *it_xVec, bgil::gray_color_t() ) = motion.x;
			bgil::get_color( *it_yVec, bgil::gray_color_t() ) = motion.y;
		}
		if( p->progressForward() )
			return true;
	}
	return false;
}

template<typename GView, typename View, typename Point, typename Scalar>
bool correlateMotionVectors( GView& xGradientView, GView& yGradientView, View& img, const Point& topleft,
                             const boost::gil::kernel_1d<Scalar>& kernel, const boost::gil::convolve_boundary_option boundary_option,
                             tuttle::plugin::IProgress* p )
{
	typedef typename GView::value_type GPixel;
	using namespace boost::gil;
	correlate_rows<GPixel>( color_converted_view<GPixel>( img ), kernel, xGradientView, topleft, boundary_option );
	if( p->progressForward( xGradientView.height() ) )
		return true;
	correlate_cols<GPixel>( color_converted_view<GPixel>( img ), kernel, yGradientView, topleft, boundary_option );
	if( p->progressForward( yGradientView.height() ) )
		return true;
	return false;
}

template<typename GView, typename View, typename Point, typename Scalar>
bool correlateMotionVectors( GView& xGradientView, GView& yGradientView, View& img, const Point& topleft,
                             const boost::gil::kernel_1d<Scalar>& kernel, const boost::gil::kernel_1d<Scalar>& kernelSecondary, const boost::gil::convolve_boundary_option boundary_option,
                             tuttle::plugin::IProgress* p )
{
	typedef typename GView::value_type GPixel;
	using namespace boost::gil;
	correlate_rows_cols<GPixel>( color_converted_view<GPixel>( img ), kernel, kernelSecondary, xGradientView, topleft, boundary_option );
	if( p->progressForward( xGradientView.height() ) )
		return true;
	correlate_rows_cols<GPixel>( color_converted_view<GPixel>( img ), kernelSecondary, kernel, yGradientView, topleft, boundary_option );
	if( p->progressForward( yGradientView.height() ) )
		return true;
	return false;
}

/**
 * @brief Moves the pixels based on the variation of the mask (the derivative: [-1 0 1] kernel)
 */
template <typename Sampler, // Models SamplerConcept
          typename SrcView, // Models RandomAccess2DImageViewConcept
          typename VecView, // Models RandomAccess2DImageViewConcept
          typename DstView>
// Models MutableRandomAccess2DImageViewConcept
bool motionvectors_resample_pixels( const SrcView& srcView, const OfxRectI& srcRod,
                                    const VecView& xVecView, const VecView& yVecView, const OfxRectI& vecRod,
                                    const DstView& dstView, const OfxRectI& dstRod,
                                    const OfxRectI& procWindowRoW,
                                    tuttle::plugin::IProgress* p,
                                    Sampler sampler = Sampler() )
{
	BOOST_ASSERT( srcView.width() == srcRod.x2 - srcRod.x1 );
	BOOST_ASSERT( srcView.height() == srcRod.y2 - srcRod.y1 );

	BOOST_ASSERT( xVecView.width() == vecRod.x2 - vecRod.x1 );
	BOOST_ASSERT( xVecView.height() == vecRod.y2 - vecRod.y1 );

	BOOST_ASSERT( yVecView.width() == xVecView.width() );
	BOOST_ASSERT( yVecView.height() == xVecView.height() );

	BOOST_ASSERT( dstView.width() == dstRod.x2 - dstRod.x1 );
	BOOST_ASSERT( dstView.height() == dstRod.y2 - dstRod.y1 );

	typedef typename DstView::point_t Point2Integer;
	typedef typename boost::gil::channel_type<VecView>::type::base_channel_t VecChannel;
	typedef typename boost::gil::point2<VecChannel> VecPoint2;
	typedef typename DstView::coord_t Coord;
	typedef typename DstView::value_type DstPixel;

	DstPixel black;
	color_convert( bgil::rgba32f_pixel_t( 0.0, 0.0, 0.0, 0.0 ), black );

	// shift between the procWindow and the output clip RoD
	// __________________________
	// |\        dst RoD         |
	// | \_________________      |
	// |  |   procWindow   |     |
	// |  |                |     |
	// |  |                |     |
	// |  |________________|     |
	// |                    \    |
	// |                      \  |
	// |________________________\|
	// procWindow is necessarily contained in dst RoD
	//
	OfxRectI shiftProcWinDstRod; // only positive values
	shiftProcWinDstRod.x1 = procWindowRoW.x1 - dstRod.x1;
	shiftProcWinDstRod.y1 = procWindowRoW.y1 - dstRod.y1;
	shiftProcWinDstRod.x2 = dstRod.x2 - procWindowRoW.x2;
	shiftProcWinDstRod.y2 = dstRod.y2 - procWindowRoW.y2;
	OfxRectI shiftProcWinVecRod;
	shiftProcWinVecRod.x1 = procWindowRoW.x1 - vecRod.x1;
	shiftProcWinVecRod.y1 = procWindowRoW.y1 - vecRod.y1;
	shiftProcWinVecRod.x2 = vecRod.x2 - procWindowRoW.x2;
	shiftProcWinVecRod.y2 = vecRod.y2 - procWindowRoW.y2;

	for( Coord y = procWindowRoW.y1; y < procWindowRoW.y2; ++y )
	{
		const Coord yDst = y - dstRod.y1;
		const Coord ySrc = y - srcRod.y1;
		const Coord yVec = y - vecRod.y1;
		typename DstView::x_iterator xit_dst  = dstView.x_at( shiftProcWinDstRod.x1, yDst );
		typename VecView::x_iterator xit_xVec = xVecView.x_at( shiftProcWinVecRod.x1, yVec );
		typename VecView::x_iterator xit_yVec = yVecView.x_at( shiftProcWinVecRod.x1, yVec );
		for( Coord x = procWindowRoW.x1;
		     x < procWindowRoW.x2;
		     ++x, ++xit_dst, ++xit_xVec, ++xit_yVec )
		{
			const Coord xSrc = x - srcRod.x1;
			const VecPoint2 pos( xSrc, ySrc );

			VecPoint2 motion;
			if( x < vecRod.x1 || x > vecRod.x2 ||
			    y < vecRod.y1 || y > vecRod.y2 )
			{
				motion.x = 0;
				motion.y = 0;
			}
			else
			{
				motion.x = bgil::get_color( *xit_xVec, bgil::gray_color_t() );
				motion.y = bgil::get_color( *xit_yVec, bgil::gray_color_t() );
			}

			// compute the pixel value according to the resample method
			if( !bgil::sample( sampler, srcView, pos + motion, *xit_dst ) )
			{
				*xit_dst = black; // if it is outside of the source image
			}
		}

		// notify the end of the line to inform the progress
		// and allows the host to abort
		if( p->progressForward() )
			return true;
	}
	return false;
}

}
}

#endif

