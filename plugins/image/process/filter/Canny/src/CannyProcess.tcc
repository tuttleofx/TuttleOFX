#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/common/math/rectOp.hpp>

#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/math/constants/constants.hpp>

#include <queue>
#include <utility>

#include "CannyPlugin.hpp"

namespace tuttle {
namespace plugin {
namespace canny {

template<class View>
CannyProcess<View>::CannyProcess( CannyPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
	this->setNoMultiThreading();
}

template <class View>
void CannyProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void CannyProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	namespace bgil = boost::gil;
	namespace bm = boost::math;
	typedef bgil::point2<std::ptrdiff_t> Point2;
	
	OfxRectI procWindowOutput = translateRegion( procWindowRoW, this->_dstPixelRod );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
	                             procWindowRoW.y2 - procWindowRoW.y1 };
	static const unsigned int border = 1;
	OfxRectI srcRodCrop = rectangleReduce( this->_srcPixelRod, border );
	OfxRectI procWindowRoWCrop = rectanglesIntersection( procWindowRoW, srcRodCrop );
	OfxRectI procWindowOutputCrop = translateRegion( procWindowRoWCrop, this->_dstPixelRod );
	OfxPointI procWindowCropSize = { procWindowRoWCrop.x2 - procWindowRoWCrop.x1,
	                                 procWindowRoWCrop.y2 - procWindowRoWCrop.y1 };
	OfxPointI srcShift = { this->_dstPixelRod.x1 - this->_srcPixelRod.x1,
	                       this->_dstPixelRod.y1 - this->_srcPixelRod.y1 };

	if( _params._border == eParamBorderBlack )
	{
		View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
												  procWindowSize.x, procWindowSize.y );

		// fill borders
		Pixel pixelZero; bgil::pixel_zeros_t<Pixel>()( pixelZero );
		boost::gil::fill_pixels( dst, pixelZero );
	}


	/**
	 * Computation of gradient norm local maxima in regard of gradient direction
	 *
	 *     there are 4 cases:
	 *
	 *                          The X marks the pixel in question, and each
	 *          C     B         of the quadrants for the gradient vector
	 *        O----0----0       fall into two cases, divided by the 45
	 *      D |         | A     degree line.  In one case the gradient
	 *        |         |       vector is more horizontal, and in the other
	 *        O    X    O       it is more vertical.  There are eight
	 *        |         |       divisions, but for the non-maximum suppression
	 *     (A)|         |(D)    we are only worried about 4 of them since we
	 *        O----O----O       use symmetric points about the center pixel.
	 *         (B)   (C)
	 */
	{
		//bouding angles for changing masks of maxima detection
//		static const double part  = bm::constants::pi<double>() / 8.0;
//		static const double angleA = 1 * part;
//		static const double angleB = 3 * part;
//		static const double angleC = 5 * part;
//		static const double angleD = 7 * part;

		const Point2 nextLine( -procWindowCropSize.x, 1 );
		typename View::xy_locator src_loc = this->_srcView.xy_at( procWindowOutputCrop.x1 + srcShift.x, procWindowOutputCrop.y1 + srcShift.y );

		// LT CT RT
		// LC    RC
		// LB CB RB
		typename View::xy_locator::cached_location_t srcLT = src_loc.cache_location(-1,-1);
		typename View::xy_locator::cached_location_t srcCT = src_loc.cache_location(0,-1);
		typename View::xy_locator::cached_location_t srcRT = src_loc.cache_location(1,-1);
		typename View::xy_locator::cached_location_t srcLC = src_loc.cache_location(-1,0);
		typename View::xy_locator::cached_location_t srcRC = src_loc.cache_location(1,0);
		typename View::xy_locator::cached_location_t srcLB = src_loc.cache_location(-1,1);
		typename View::xy_locator::cached_location_t srcCB = src_loc.cache_location(0,1);
		typename View::xy_locator::cached_location_t srcRB = src_loc.cache_location(1,1);

		for( int y = procWindowOutputCrop.y1;
				 y < procWindowOutputCrop.y2;
				 ++y )
		{
			typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutputCrop.x1, y );
			for( int x = procWindowOutputCrop.x1;
				 x < procWindowOutputCrop.x2;
				 ++x, ++src_loc.x(), ++dst_it )
			{
				bgil::pixel_zeros_t<Pixel>()( *dst_it );
				static const unsigned int vecX = 0;
				static const unsigned int vecY = 1;
				static const unsigned int norm = 2;
//				static const unsigned int direction = 3;

				// particular case for each quadrant
//				A:
//				if( (*src_loc)[direction] > angleD ||
//					(*src_loc)[direction] < angleA )
				if( ((*src_loc)[vecY] <= 0 && (*src_loc)[vecX] > -(*src_loc)[vecY]) ||
				    ((*src_loc)[vecY] >= 0 && (*src_loc)[vecX] < -(*src_loc)[vecY]) )
				{
					if( (*src_loc)[norm] > src_loc[srcLC][norm] &&
						(*src_loc)[norm] > src_loc[srcRC][norm] )
						(*dst_it)[0] = (*src_loc)[norm];
					else
						(*dst_it)[0] = 0;
				}
//				B:
				else if ( ((*src_loc)[vecX] > 0 && -(*src_loc)[vecY] >= (*src_loc)[vecX]) ||
				          ((*src_loc)[vecX] < 0 && -(*src_loc)[vecY] <= (*src_loc)[vecX]) )
//				else if( (*src_loc)[direction] > angleC )
				{
					if( (*src_loc)[norm] > src_loc[srcLB][norm] &&
						(*src_loc)[norm] > src_loc[srcRT][norm] )
						(*dst_it)[0] = (*src_loc)[norm];
					else
						(*dst_it)[0] = 0;
				}
//				C:
				else if( ((*src_loc)[vecX] <= 0 && (*src_loc)[vecX] > (*src_loc)[vecY]) ||
				         ((*src_loc)[vecX] >= 0 && (*src_loc)[vecX] < (*src_loc)[vecY]) )
//				else if( (*src_loc)[direction] > angleB )
				{
					if( (*src_loc)[norm] > src_loc[srcCB][norm] &&
						(*src_loc)[norm] > src_loc[srcCT][norm] )
						(*dst_it)[0] = (*src_loc)[norm];
					else
						(*dst_it)[0] = 0;
				}
//				D:
//				else if( ((*src_loc)[vecY] < 0 && (*src_loc)[vecX] <= (*src_loc)[vecY]) ||
//				         ((*src_loc)[vecY] > 0 && (*src_loc)[vecX] >= (*src_loc)[vecY]) )
				else
				{
					if( (*src_loc)[norm] > src_loc[srcRB][norm] &&
						(*src_loc)[norm] > src_loc[srcLT][norm] )
						(*dst_it)[0] = (*src_loc)[norm];
					else
						(*dst_it)[0] = 0;
				}
			}
			src_loc += nextLine;

			if( this->progressForward() )
				return;
		}
	}
	
	// Hysteresis by flooding
	if( _params._hysteresis )
	{
		OfxRectI rectLimit = rectangleReduce(procWindowOutputCrop,1);
		const Point2 nextLine( -procWindowCropSize.x, 1 );
		typename View::xy_locator dst_loc = this->_dstView.xy_at( procWindowOutputCrop.x1, procWindowOutputCrop.y1 );
		for( int y = procWindowOutputCrop.y1;
				 y < procWindowOutputCrop.y2;
				 ++y )
		{
			for( int x = procWindowOutputCrop.x1;
				 x < procWindowOutputCrop.x2;
				 ++x, ++dst_loc.x() )
			{
				if( (*dst_loc)[0] >= _params._upperThres )
				{
					(*dst_loc)[1] = bgil::channel_traits<Channel>::max_value();
					std::queue<Point2> fifo; ///< @todo tuttle: use host allocator
					fifo.push( Point2( x, y ) );
					while( !fifo.empty() )
					{
						const Point2 p = fifo.front();
						fifo.pop();
						for( int dy = -1; dy < 2; ++dy )
						{
							for( int dx = -1; dx < 2; ++dx )
							{
								typename View::iterator pix = this->_dstView.at( p.x+dx, p.y+dy );
								if( (*pix)[0] > _params._lowerThres &&
								    (*pix)[2] != bgil::channel_traits<Channel>::max_value() )
								{
									(*pix)[2] = bgil::channel_traits<Channel>::max_value();
									if( (*pix)[0] < _params._upperThres )
									{
										Point2 np( p.x+dx, p.y+dy );
										// inside a subwindow of the rendering,
										// we can't append border pixels
										if( pointInRect( np, rectLimit ) )
											fifo.push( np );
									}
								}
							}
						}
					}
				}
			}
			dst_loc += nextLine;
		}
	}

	if( _params._fillAllChannels )
	{
		View tmp_dst = subimage_view( this->_dstView, procWindowOutputCrop.x1, procWindowOutputCrop.y1,
												  procWindowCropSize.x, procWindowCropSize.y );
		bgil::copy_pixels( bgil::kth_channel_view<2>(tmp_dst), bgil::kth_channel_view<0>(tmp_dst) );
		bgil::copy_pixels( bgil::kth_channel_view<2>(tmp_dst), bgil::kth_channel_view<1>(tmp_dst) );
		bgil::copy_pixels( bgil::kth_channel_view<2>(tmp_dst), bgil::kth_channel_view<3>(tmp_dst) );
	}
}

}
}
}
