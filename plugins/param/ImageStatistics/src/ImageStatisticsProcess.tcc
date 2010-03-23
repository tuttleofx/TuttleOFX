#include "ImageStatisticsPlugin.hpp"
#include "ImageStatisticsProcess.hpp"
#include <tuttle/common/image/gilGlobals.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations2.hpp>

namespace boost {
namespace gil {

////////////////////////////////////////////////////////////////////////////////

/// \ingroup ChannelNumericOperations
/// \brief ch2 = min( ch1, ch2 )
/// structure for adding one channel to another
/// this is a generic implementation; user should specialize it for better performance
template <typename ChannelSrc,typename ChannelDst>
struct channel_assign_min_t : public std::binary_function<ChannelSrc,ChannelDst,ChannelDst>
{
    typename channel_traits<ChannelDst>::reference
	operator()( typename channel_traits<ChannelSrc>::const_reference ch1,
                typename channel_traits<ChannelDst>::reference ch2 ) const
	{
        return ch2 = std::min( ChannelDst( ch1 ), ch2 );
    }
};

/// \ingroup PixelNumericOperations
/// \brief p2 = min( p1, p2 )
template <typename PixelSrc, // models pixel concept
          typename PixelDst> // models pixel value concept
struct pixel_assign_min_t
{
    PixelDst& operator()( const PixelSrc& p1,
                          PixelDst& p2 ) const
	{
        static_for_each( p1, p2,
                         channel_assign_min_t<typename channel_type<PixelSrc>::type,
                                              typename channel_type<PixelDst>::type>() );
        return p2;
    }
};

////////////////////////////////////////////////////////////////////////////////

/// \ingroup ChannelNumericOperations
/// \brief ch2 = max( ch1, ch2 )
/// this is a generic implementation; user should specialize it for better performance
template <typename ChannelSrc,typename ChannelDst>
struct channel_assign_max_t : public std::binary_function<ChannelSrc,ChannelDst,ChannelDst>
{
    typename channel_traits<ChannelDst>::reference
	operator()( typename channel_traits<ChannelSrc>::const_reference ch1,
                typename channel_traits<ChannelDst>::reference ch2 ) const
	{
        return ch2 = std::max( ChannelDst( ch1 ), ch2 );
    }
};

/// \ingroup PixelNumericOperations
/// \brief p2 = max( p1, p2 )
template <typename PixelSrc, // models pixel concept
          typename PixelDst> // models pixel value concept
struct pixel_assign_max_t
{
    PixelDst& operator()( const PixelSrc& p1,
                          PixelDst& p2 ) const
	{
        static_for_each( p1, p2,
                         channel_assign_max_t<typename channel_type<PixelSrc>::type,
                                              typename channel_type<PixelDst>::type>() );
        return p2;
    }
};

////////////////////////////////////////////////////////////////////////////////

}
}



namespace tuttle {
namespace plugin {
namespace imageStatistics {

template<class View>
ImageStatisticsProcess<View>::ImageStatisticsProcess( ImageStatisticsPlugin &instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
{
	this->setNoMultiThreading( );
}

template<class View>
void ImageStatisticsProcess<View>::setup( const OFX::RenderArguments &args )
{
	using namespace boost::gil;

	ImageGilFilterProcessor<View>::setup( args );

	// recovery parameters values
	OfxRectI srcRod = this->_srcClip->getPixelRod( args.time );
	_processParams = _plugin.getProcessParams( srcRod );

	Point2 rectSize( std::abs( _processParams._rect.x2 - _processParams._rect.x1 ),
					 std::abs( _processParams._rect.y2 - _processParams._rect.y1 ) );

	COUT_VAR( _processParams._rect );
	COUT_VAR( rectSize );

	typedef pixel<bits32f, layout<typename color_space_type<View>::type> > Pixel32f;
	typedef pixel<typename channel_type<View>::type, layout<gray_t> > PixelGray;

	// declare values and init
	Pixel firstPixel = this->_srcView(_processParams._rect.x1, _processParams._rect.y1); // for initialization only
	PixelGray firstPixelGray;
	color_convert( firstPixel, firstPixelGray );

	Pixel32f average;
	pixel_zeros_t<Pixel32f>()( average );
	Pixel minChannel = firstPixel;
	Pixel maxChannel = firstPixel;
	Pixel minLuminosity = firstPixel;
	PixelGray minLuminosityGray = firstPixelGray;
	Pixel maxLuminosity = firstPixel;
	PixelGray maxLuminosityGray = firstPixelGray;

	for( int y = _processParams._rect.y1;
		 y < _processParams._rect.y2;
		 ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( _processParams._rect.x1, y );
		Pixel32f lineAverage;
		pixel_assigns_t<Pixel, Pixel32f>()( *(src_it++), lineAverage ); // init with first value
		
		for( int x = _processParams._rect.x1+1;
			 x < _processParams._rect.x2;
			 ++x, ++src_it )
		{
			// for average : accumulate
			pixel_plus_assign_t<Pixel, Pixel32f>()( ( *src_it ), lineAverage ); // lineAverage += src_it;

			// search min for each channel
			pixel_assign_min_t<Pixel, Pixel>()( *src_it, minChannel );
			// search max for each channel
			pixel_assign_max_t<Pixel, Pixel>()( *src_it, maxChannel );
			
			PixelGray grayCurrentPixel; // current pixel in gray colorspace
			color_convert( *src_it, grayCurrentPixel );
			// search min luminosity
			if( get_color( grayCurrentPixel, gray_color_t() ) < get_color( minLuminosityGray, gray_color_t() ) )
			{
				minLuminosity = *src_it;
				minLuminosityGray = grayCurrentPixel;
			}
			// search max luminosity
			if( get_color( grayCurrentPixel, gray_color_t() ) > get_color( maxLuminosityGray, gray_color_t() ) )
			{
				maxLuminosity = *src_it;
				maxLuminosityGray = grayCurrentPixel;
			}
		}
		// for average : divide by number of accumulated pixels
		pixel_divides_scalar_assign_t<double, Pixel32f>()( rectSize.x, lineAverage ); // lineAverage /= rectSize.x;
		// for average : accumulate each line
		pixel_plus_assign_t<Pixel32f, Pixel32f>()( lineAverage, average ); // _average += lineAverage;
	}
	// for average : divide by number of accumulated lines
	pixel_divides_scalar_assign_t<double, Pixel32f>()( rectSize.y, average ); // _average /= rectSize.y;


	rgba32f_pixel_t paramRgbaValue;
	color_convert( average, paramRgbaValue );
	_plugin._outputAverage->setValueAtTime( args.time,
	                                        get_color( paramRgbaValue, red_t() ),
	                                        get_color( paramRgbaValue, green_t() ),
	                                        get_color( paramRgbaValue, blue_t() ),
	                                        get_color( paramRgbaValue, alpha_t() )
	                                      );
	color_convert( minChannel, paramRgbaValue );
	_plugin._outputChannelMin->setValueAtTime( args.time,
	                                        get_color( paramRgbaValue, red_t() ),
	                                        get_color( paramRgbaValue, green_t() ),
	                                        get_color( paramRgbaValue, blue_t() ),
	                                        get_color( paramRgbaValue, alpha_t() )
	                                      );
	color_convert( maxChannel, paramRgbaValue );
	_plugin._outputChannelMax->setValueAtTime( args.time,
	                                        get_color( paramRgbaValue, red_t() ),
	                                        get_color( paramRgbaValue, green_t() ),
	                                        get_color( paramRgbaValue, blue_t() ),
	                                        get_color( paramRgbaValue, alpha_t() )
	                                      );
	color_convert( minLuminosity, paramRgbaValue );
	_plugin._outputLuminosityMin->setValueAtTime( args.time,
	                                        get_color( paramRgbaValue, red_t() ),
	                                        get_color( paramRgbaValue, green_t() ),
	                                        get_color( paramRgbaValue, blue_t() ),
	                                        get_color( paramRgbaValue, alpha_t() )
	                                      );
	color_convert( maxLuminosity, paramRgbaValue );
	_plugin._outputLuminosityMax->setValueAtTime( args.time,
	                                        get_color( paramRgbaValue, red_t() ),
	                                        get_color( paramRgbaValue, green_t() ),
	                                        get_color( paramRgbaValue, blue_t() ),
	                                        get_color( paramRgbaValue, alpha_t() )
	                                      );
	
	switch( _processParams._chooseOutput )
	{
		case eChooseOutputSource:
			break;
		case eChooseOutputAverage:
			color_convert( average, _outputPixel );
			break;
		case eChooseOutputChannelMin:
			_outputPixel = minChannel;
			break;
		case eChooseOutputChannelMax:
			_outputPixel = maxChannel;
			break;
		case eChooseOutputLuminosityMin:
			_outputPixel = minLuminosity;
			break;
		case eChooseOutputLuminosityMax:
			_outputPixel = maxLuminosity;
			break;
	}
}

/**
 * @param[in] procWindow  Processing window
 */
template<class View>
void ImageStatisticsProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	using namespace boost::gil;
	if( _processParams._chooseOutput == eChooseOutputSource )
	{
		for( int y = procWindow.y1;
			 y < procWindow.y2;
			 ++y )
		{
			typename View::x_iterator src_it = this->_srcView.x_at( procWindow.x1, y );
			typename View::x_iterator dst_it = this->_dstView.x_at( procWindow.x1, y );
			for( int x = procWindow.x1;
				 x < procWindow.x2;
				 ++x, ++src_it, ++dst_it )
			{
				(*dst_it) = (*src_it);
			}
			if( this->progressForward() )
				return;
		}
	}
	else
	{
		for( int y = procWindow.y1;
			 y < procWindow.y2;
			 ++y )
		{
			typename View::x_iterator dst_it = this->_dstView.x_at( procWindow.x1, y );
			for( int x = procWindow.x1;
				 x < procWindow.x2;
				 ++x, ++dst_it )
			{
				( *dst_it ) = _outputPixel;
			}
			if( this->progressForward( ) )
				return;
		}
	}
}

}
}
}
