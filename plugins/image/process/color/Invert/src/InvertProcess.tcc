#include "InvertAlgorithm.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/algorithm.hpp>
#include <boost/gil/extension/channel.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

#include <cmath>
#include <vector>

namespace tuttle {
namespace plugin {
namespace invert {

template<class View>
InvertProcess<View>::InvertProcess( InvertPlugin& instance )
	: ImageGilFilterProcessor<View>( instance )
	, _plugin( instance )
{}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void InvertProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x, procWindowSize.y );

//	if( params._alpha )
//	{
//		transform_pixels_progress(
//			src,
//			dst,
//			transform_pixel_by_channel_t<channel_invert_t>(),
//			*this
//			);
//	}
//	else
//	{
		transform_pixels_progress(
			src,
			dst,
			pixel_invert_colors_t(),
			*this
			);
//	}

}

}
}
}
