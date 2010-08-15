#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>


namespace tuttle {
namespace plugin {
namespace colorspace {
namespace log2lin {

using namespace boost::gil;

namespace {
static const float cineonBlackOffset = std::pow(10.0f, (95.0f-685.0f) / 300.0f);

inline float fromCineon(float x)
{
	return (std::pow(10.0f,(1023.0f*x-685.0f)/300.0f)-cineonBlackOffset)/(1.0f-cineonBlackOffset);
}
}

template<class View>
Log2LinProcess<View>::Log2LinProcess( Log2LinPlugin &instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
{
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void Log2LinProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
							     procWindowRoW.y2 - procWindowRoW.y1 };

	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x,
							  procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
							  procWindowSize.x,
							  procWindowSize.y );

	log2lin(src, dst);
}

template<class View>
void Log2LinProcess<View>::log2lin(View & src, View & dst)
{
	typedef pixel<bits32f, layout<typename color_space_type<View>::type> > PixelF;
	PixelF p;
	int ncMax = num_channels<View>::value;
	bool processAlpha = false;
	if (!processAlpha && num_channels<View>::value == 4)
	{
		ncMax = 3;
	}

	for( int y = 0; y < src.height(); ++y )
	{
		typename View::x_iterator src_it = src.row_begin( y );
		typename View::x_iterator dst_it = dst.row_begin( y );

		for( int x = 0; x < src.width(); ++x, ++src_it, ++dst_it )
		{
			color_convert(*src_it, p);
			for(int c = 0; c < ncMax; ++c)
			{
				p[c] = fromCineon(p[c]);
			}
			color_convert(p, *dst_it);
		}
		if( this->progressForward() )
			return;
	}
}

}
}
}
}
