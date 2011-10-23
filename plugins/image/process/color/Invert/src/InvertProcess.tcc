#include "InvertAlgorithm.hpp"

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include <terry/globals.hpp>
#include <tuttle/plugin/image/algorithm.hpp>

#include <terry/numeric/pixel_by_channel.hpp>
#include <terry/channel_view.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/rgba.hpp>

namespace tuttle {
namespace plugin {
namespace invert {

template<class View>
InvertProcess<View>::InvertProcess( InvertPlugin& instance )
	: ImageGilFilterProcessor<View>( instance )
	, _plugin( instance )
{
}
template<class View>
void InvertProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void InvertProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	using namespace terry;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );
	OfxPointI procWindowSize  = {
		procWindowRoW.x2 - procWindowRoW.x1,
		procWindowRoW.y2 - procWindowRoW.y1
	};

	View src = subimage_view( this->_srcView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x, procWindowSize.y );
	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
	                          procWindowSize.x, procWindowSize.y );

	if( _params._red &&
	    _params._green &&
	    _params._blue &&
	    _params._alpha )
	{
		transform_pixels_progress(
			src,
			dst,
			transform_pixel_by_channel_t<channel_invert_t>(),
			*this
			);
	}
	else if( _params._red &&
	    _params._green &&
	    _params._blue &&
	    !_params._alpha )
	{
		transform_pixels_progress(
			src,
			dst,
			pixel_invert_colors_t(),
			*this
			);
	}
	else
	{
		/// @todo tuttle: add a templated function to support Gray images.
		{
			typedef red_t LocalChannel;
			if( _params._red )
			{
				transform_pixels_progress(
					channel_view<LocalChannel,View>(src),
					channel_view<LocalChannel,View>(dst),
					transform_pixel_by_channel_t<channel_invert_t>(),
					*this
					);
			}
			else
			{
				copy_pixels( channel_view<LocalChannel>(src), channel_view<LocalChannel>(dst) );
			}
		}
		{
			typedef green_t LocalChannel;
			if( _params._green )
			{
				transform_pixels_progress(
					channel_view<LocalChannel,View>(src),
					channel_view<LocalChannel,View>(dst),
					transform_pixel_by_channel_t<channel_invert_t>(),
					*this
					);
			}
			else
			{
				copy_pixels( channel_view<LocalChannel>(src), channel_view<LocalChannel>(dst) );
			}
		}
		{
			typedef blue_t LocalChannel;
			if( _params._blue )
			{
				transform_pixels_progress(
					channel_view<LocalChannel,View>(src),
					channel_view<LocalChannel,View>(dst),
					transform_pixel_by_channel_t<channel_invert_t>(),
					*this
					);
			}
			else
			{
				copy_pixels( channel_view<LocalChannel>(src), channel_view<LocalChannel>(dst) );
			}
		}
		{
			typedef alpha_t LocalChannel;
			if( _params._alpha )
			{
				transform_pixels_progress(
					channel_view<LocalChannel,View>(src),
					channel_view<LocalChannel,View>(dst),
					transform_pixel_by_channel_t<channel_invert_t>(),
					*this
					);
			}
			else
			{
				copy_pixels( channel_view<LocalChannel>(src), channel_view<LocalChannel>(dst) );
			}
		}
	}

}

}
}
}
