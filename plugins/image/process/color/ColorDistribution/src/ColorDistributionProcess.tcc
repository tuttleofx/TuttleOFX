#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/PluginException.hpp>

namespace tuttle {
namespace plugin {
namespace colorDistribution {

template<class View>
ColorDistributionProcess<View>::ColorDistributionProcess( ColorDistributionPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void ColorDistributionProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	ColorDistributionProcessParams<ColorDistributionPlugin::Scalar>	params = _plugin.getProcessParams();
	rgb32f_pixel_t wpix;
	double inv24 = 1.0/2.4;
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
			color_convert(*src_it, wpix);

			// Lin to sRGB
			// Formula taken from Computational technology by Kang.
			if ( params.invert == true )
			{
				if ( wpix[ 0 ] > 0.00304 )
				{
					 wpix[ 0 ] = 1.055 * exp( log( wpix[ 0 ] ) * inv24 ) - 0.055;
				}
				else
				{
					wpix[ 0 ] = 12.92 * wpix[ 0 ];
				}
				if ( wpix[ 1 ] > 0.00304 )
				{
					 wpix[ 1 ] = 1.055 * exp( log( wpix[ 1 ] ) * inv24 ) - 0.055;
				}
				else
				{
					wpix[ 1 ] = 12.92 * wpix[ 1 ];
				}
				if ( wpix[ 2 ] > 0.00304 )
				{
					 wpix[ 2 ] = 1.055 * exp( log( wpix[ 2 ] ) * inv24 ) - 0.055;
				}
				else
				{
					wpix[ 2 ] = 12.92 * wpix[ 2 ];
				}
			}
			else
			{
				if ( wpix[ 0 ] > 0.03928 )
				{
					wpix[ 0 ] = exp( log( (wpix[ 0 ] + 0.055) / 1.055 ) * 2.4 );
				}
				else
				{
					wpix[ 0 ] = wpix[ 0 ] / 12.92;
				}
				if ( wpix[ 1 ] > 0.03928 )
				{
					wpix[ 1 ] = exp( log( (wpix[ 1 ] + 0.055) / 1.055 ) * 2.4 );
				}
				else
				{
					wpix[ 1 ] = wpix[ 1 ] / 12.92;
				}
				if ( wpix[ 2 ] > 0.03928 )
				{
					wpix[ 2 ] = exp( log( (wpix[ 2 ] + 0.055) / 1.055 ) * 2.4 );
				}
				else
				{
					wpix[ 2] = wpix[ 2 ] / 12.92;
				}
			}
			color_convert(wpix, *dst_it);
		}
		if( this->progressForward() )
			return;
	}
	/*
	const OfxRectI procWindowSrc = this->translateRegion( procWindowRoW, this->_srcPixelRod );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };
	View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
							  procWindowSize.x,
							  procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x,
							  procWindowSize.y );
	copy_pixels( src, dst );
	*/

}

}
}
}
