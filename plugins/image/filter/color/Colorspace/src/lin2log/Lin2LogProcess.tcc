#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <limits>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>


namespace tuttle {
namespace plugin {
namespace colorspace {
namespace lin2log {

using namespace boost::gil;

static const float cineonBlackOffset = powf(10.0f, (95.0f-685.0f) / 300.0f);

inline float toCineon(float v)
{
	float t = v*(1.0f-cineonBlackOffset)+cineonBlackOffset;
	if (t <= 0.0f) return -std::numeric_limits<float>::infinity();
	return (log10f(t)*300.0f+685.0f)/1023.0f;
}

struct lin2loger
{
	inline rgba32f_pixel_t operator()( const rgba32f_pixel_t& p ) const
	{
		rgba32f_pixel_t p2;
		for( int n = 0; n < 3; ++n )
		{
			p2[n] = toCineon(p[n]);
		}
		p2[3] = p[3];
		return p2;
	}
};

template<class View>
Lin2LogProcess<View>::Lin2LogProcess( Lin2LogPlugin &instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
{
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void Lin2LogProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
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

	// Put your computations here
	transform_pixels( color_converted_view<rgba32f_pixel_t>(src),  color_converted_view<rgba32f_pixel_t>(dst), lin2loger() );
}

}
}
}
}
