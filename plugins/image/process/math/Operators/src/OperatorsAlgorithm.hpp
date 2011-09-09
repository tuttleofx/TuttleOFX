#ifndef _TUTTLE_PLUGIN_OPERATORS_ALGORITHM_HPP_
#define _TUTTLE_PLUGIN_OPERATORS_ALGORITHM_HPP_

#include <boost/gil/extension/channel.hpp>

namespace tuttle {
namespace plugin {
namespace operators {

namespace bgil = boost::gil;

template< class View >
void OperatorsProcess<View>::processImagePlus( OfxRectI& procWindowOutput, const OfxRectI& procWindowRoW, const boost::gil::rgba32f_pixel_t& values, const boost::gil::rgba8_pixel_t& processChannel )
{
	boost::gil::rgba32f_pixel_t wpix;
	for( int y = procWindowOutput.y1;
	     y < procWindowOutput.y2;
	     ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
		     x < procWindowOutput.x2;
		     ++x, ++src_it, ++dst_it )
		{
			color_convert( *src_it, wpix );

			if( processChannel[ 0 ] == 1.0 )
				wpix[ 0 ] = wpix[ 0 ] + values[ 0 ] ;
			if( processChannel[ 1 ] == 1.0 )
				wpix[ 1 ] = wpix[ 1 ] + values[ 1 ] ;
			if( processChannel[ 2 ] == 1.0 )
				wpix[ 2 ] = wpix[ 2 ] + values[ 2 ] ;
			if( processChannel[ 3 ] == 1.0 )
				wpix[ 3 ] = wpix[ 3 ] + values[ 3 ] ;
			
			color_convert( wpix, *dst_it );
		}
		if( this->progressForward() )
			return;
	}
}

template< class View >
void OperatorsProcess<View>::processImageMultiply( OfxRectI& procWindowOutput, const OfxRectI& procWindowRoW, const boost::gil::rgba32f_pixel_t& values, const boost::gil::rgba8_pixel_t& processChannel )
{
	boost::gil::rgba32f_pixel_t wpix;
	for( int y = procWindowOutput.y1;
	     y < procWindowOutput.y2;
	     ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
		     x < procWindowOutput.x2;
		     ++x, ++src_it, ++dst_it )
		{
			color_convert( *src_it, wpix );
			
			if( processChannel[ 0 ] == 1.0 )
				wpix[ 0 ] = wpix[ 0 ] * values[ 0 ] ;
			if( processChannel[ 1 ] == 1.0 )
				wpix[ 1 ] = wpix[ 1 ] * values[ 1 ] ;
			if( processChannel[ 2 ] == 1.0 )
				wpix[ 2 ] = wpix[ 2 ] * values[ 2 ] ;
			if( processChannel[ 3 ] == 1.0 )
				wpix[ 3 ] = wpix[ 3 ] * values[ 3 ] ;
			
			color_convert( wpix, *dst_it );
		}
		if( this->progressForward() )
			return;
	}
}

template< class View >
void OperatorsProcess<View>::processImagePow( OfxRectI& procWindowOutput, const OfxRectI& procWindowRoW, const boost::gil::rgba32f_pixel_t& values, const boost::gil::rgba8_pixel_t& processChannel )
{
	boost::gil::rgba32f_pixel_t wpix;
	for( int y = procWindowOutput.y1;
	     y < procWindowOutput.y2;
	     ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
		     x < procWindowOutput.x2;
		     ++x, ++src_it, ++dst_it )
		{
			color_convert( *src_it, wpix );
			
			if( processChannel[ 0 ] == 1.0 )
				wpix[ 0 ] = std::pow( wpix[ 0 ], values[ 0 ] ) ;
			if( processChannel[ 1 ] == 1.0 )
				wpix[ 1 ] = std::pow( wpix[ 1 ], values[ 1 ] ) ;
			if( processChannel[ 2 ] == 1.0 )
				wpix[ 2 ] = std::pow( wpix[ 2 ], values[ 2 ] ) ;
			if( processChannel[ 3 ] == 1.0 )
				wpix[ 3 ] = std::pow( wpix[ 3 ], values[ 3 ] ) ;
			
			color_convert( wpix, *dst_it );
		}
		if( this->progressForward() )
			return;
	}
}


template< class View >
void OperatorsProcess<View>::processImageSqrt( OfxRectI& procWindowOutput, const OfxRectI& procWindowRoW, const boost::gil::rgba32f_pixel_t& values, const boost::gil::rgba8_pixel_t& processChannel )
{
	boost::gil::rgba32f_pixel_t wpix;
	for( int y = procWindowOutput.y1;
	     y < procWindowOutput.y2;
	     ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
		     x < procWindowOutput.x2;
		     ++x, ++src_it, ++dst_it )
		{
			color_convert( *src_it, wpix );
			
			if( processChannel[ 0 ] == 1.0 )
				wpix[ 0 ] = std::sqrt( wpix[ 0 ] ) ;
			if( processChannel[ 1 ] == 1.0 )
				wpix[ 1 ] = std::sqrt( wpix[ 1 ] ) ;
			if( processChannel[ 2 ] == 1.0 )
				wpix[ 2 ] = std::sqrt( wpix[ 2 ] ) ;
			if( processChannel[ 3 ] == 1.0 )
				wpix[ 3 ] = std::sqrt( wpix[ 3 ] ) ;
			
			color_convert( wpix, *dst_it );
		}
		if( this->progressForward() )
			return;
	}
}

template< class View >
void OperatorsProcess<View>::processImageLn( OfxRectI& procWindowOutput, const OfxRectI& procWindowRoW, const boost::gil::rgba32f_pixel_t& values, const boost::gil::rgba8_pixel_t& processChannel )
{
	boost::gil::rgba32f_pixel_t wpix;
	for( int y = procWindowOutput.y1;
	     y < procWindowOutput.y2;
	     ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
		     x < procWindowOutput.x2;
		     ++x, ++src_it, ++dst_it )
		{
			color_convert( *src_it, wpix );
			
			if( processChannel[ 0 ] == 1.0 )
				wpix[ 0 ] = std::log( wpix[ 0 ] ) ;
			if( processChannel[ 1 ] == 1.0 )
				wpix[ 1 ] = std::log( wpix[ 1 ] ) ;
			if( processChannel[ 2 ] == 1.0 )
				wpix[ 2 ] = std::log( wpix[ 2 ] ) ;
			if( processChannel[ 3 ] == 1.0 )
				wpix[ 3 ] = std::log( wpix[ 3 ] ) ;
			
			color_convert( wpix, *dst_it );
		}
		if( this->progressForward() )
			return;
	}
}


template< class View >
void OperatorsProcess<View>::processImageLog( OfxRectI& procWindowOutput, const OfxRectI& procWindowRoW, const boost::gil::rgba32f_pixel_t& values, const boost::gil::rgba8_pixel_t& processChannel )
{
	boost::gil::rgba32f_pixel_t wpix;
	for( int y = procWindowOutput.y1;
	     y < procWindowOutput.y2;
	     ++y )
	{
		typename View::x_iterator src_it = this->_srcView.x_at( procWindowOutput.x1, y );
		typename View::x_iterator dst_it = this->_dstView.x_at( procWindowOutput.x1, y );
		for( int x = procWindowOutput.x1;
		     x < procWindowOutput.x2;
		     ++x, ++src_it, ++dst_it )
		{
			color_convert( *src_it, wpix );

			if( processChannel[ 0 ] == 1.0 )
				wpix[ 0 ] = std::log10( wpix[ 0 ] ) ;
			if( processChannel[ 1 ] == 1.0 )
				wpix[ 1 ] = std::log10( wpix[ 1 ] ) ;
			if( processChannel[ 2 ] == 1.0 )
				wpix[ 2 ] = std::log10( wpix[ 2 ] ) ;
			if( processChannel[ 3 ] == 1.0 )
				wpix[ 3 ] = std::log10( wpix[ 3 ] ) ;

			color_convert( wpix, *dst_it );
		}
		if( this->progressForward() )
			return;
	}
}

}
}
}

#endif
