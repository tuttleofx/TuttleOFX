#ifndef _TUTTLE_PLUGIN_FLOODFILL_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_FLOODFILL_ALGORITHM_HPP_

#include <tuttle/common/math/rectOp.hpp>

#include <boost/gil/extension/channel.hpp>

#include <queue>

namespace tuttle {
namespace plugin {
namespace floodFill {



template<class View>
void fill_pixels( View& dstView, const OfxRectI& procWindow,
				  const typename View::value_type& pixelValue )
{
	typedef typename View::value_type Pixel;

	View dst = subimage_view( dstView, procWindow.x1, procWindow.y1,
	                                   procWindow.x2-procWindow.x1, procWindow.y2-procWindow.y1 );
	boost::gil::fill_pixels( dst, pixelValue );
}




struct Connectivity4
{
	template<typename T>
	void operator()( T& first, T& last, const T& min, const T& max ) const
	{}
};

struct Connectivity8
{
	template<typename T>
	void operator()( T& first, T& last, const T& min, const T& max ) const
	{
		if( first > min )
			--first;
		if( last < max )
			++last;
	}
};

template<typename T>
struct IsUpper
{
	const T _threshold;
	IsUpper( const T& threshold )
	: _threshold(threshold)
	{}
	bool operator()( const T& p ) const
	{
		return (p >= _threshold);
	}
};


template<class ConnectivityFunctor, class StrongTest, class SoftTest, class SView, class DView>
void flood_fill( const SView& srcView, const OfxRectI& srcRod,
				 DView& dstView, const OfxRectI& dstRod,
				 const OfxRectI& procWindow,
				 const StrongTest& strongTest, const SoftTest& softTest )
{
	namespace bgil = boost::gil;
    typedef typename SView::value_type SPixel;
    typedef typename bgil::channel_type<SView>::type SChannel;
	typedef typename SView::iterator SIterator;
    typedef typename DView::value_type DPixel;
    typedef typename bgil::channel_type<DView>::type DChannel;
	typedef typename DView::iterator DIterator;
	
	typedef bgil::point2<std::ptrdiff_t> Point2;

	static const std::size_t gradMax = 0;
	static const std::size_t lower = 0;
	static const std::size_t upper = 1;
	static const std::size_t flooding = 2;

	OfxRectI procWindowOutput = translateRegion( procWindow, dstRod );
	OfxPointI procWindowSize = { procWindow.x2 - procWindow.x1,
	                             procWindow.y2 - procWindow.y1 };
	OfxRectI rectLimit = rectangleReduce(procWindow, 1);

	const Point2 nextLine( -procWindowSize.x, 1 );
	typename DView::xy_locator dst_loc = dstView.xy_at( procWindowOutput.x1, procWindowOutput.y1 );
	typename DView::xy_locator src_loc = srcView.xy_at( procWindow.x1 - srcRod.x1, procWindow.y1 - srcRod.y1 );

	for( int y = procWindow.y1;
			 y < procWindow.y2;
			 ++y )
	{
		for( int x = procWindow.x1;
			 x < procWindow.x2;
			 ++x, ++dst_loc.x(), ++src_loc.x() )
		{
			if( softTest( (*src_loc)[gradMax] ) )
			{
				(*dst_loc)[lower] = bgil::channel_traits<DChannel>::max_value();
				if( strongTest( (*src_loc)[gradMax] ) )
				{
					(*dst_loc)[upper] = bgil::channel_traits<DChannel>::max_value();
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
								DIterator pix = dstView.at( p.x+dx - dstRod.x1, p.y+dy - dstRod.y1 );
								SIterator spix = srcView.at( p.x+dx - srcRod.x1, p.y+dy - srcRod.y1 );
								if( softTest( (*spix)[gradMax] ) &&
									(*pix)[flooding] != bgil::channel_traits<DChannel>::max_value() )
								{
									(*pix)[flooding] = bgil::channel_traits<DChannel>::max_value();
									if( ! strongTest( (*spix)[gradMax] ) )
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
//			else
//			{
//				(*dst_loc)[upper] = bgil::channel_traits<DChannel>::max_value();
//			}
		}
		dst_loc += nextLine;
		src_loc += nextLine;
	}

//	if( _params._fillAllChannels )
//	{
	DView tmp_dst = subimage_view( dstView, procWindowOutput.x1, procWindowOutput.y1,
											  procWindowSize.x, procWindowSize.y );
	bgil::copy_pixels( bgil::kth_channel_view<flooding>(tmp_dst), bgil::kth_channel_view<lower>(tmp_dst) );
	bgil::copy_pixels( bgil::kth_channel_view<flooding>(tmp_dst), bgil::kth_channel_view<upper>(tmp_dst) );
	bgil::copy_pixels( bgil::kth_channel_view<flooding>(tmp_dst), bgil::kth_channel_view<3>(tmp_dst) );
//	}
}

}
}
}

#endif
