#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <terry/globals.hpp>
#include <terry/algorithm/pixel_by_channel.hpp>
#include <terry/algorithm/transform_pixels_progress.hpp>
#include <terry/color/invert.hpp>
#include <terry/channel_view.hpp>

#include <boost/gil/algorithm.hpp>
#include <boost/gil/rgba.hpp>

namespace tuttle {
namespace plugin {
namespace invert {

template<class View>
InvertProcess<View>::InvertProcess( InvertPlugin& instance )
	: ImageGilFilterProcessor<View>( instance, eImageOrientationIndependant )
	, _plugin( instance )
{
}

template<class View>
void InvertProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );
}

template<class View>
void InvertProcess<View>::preProcess()
{
	const std::size_t nbChannels = boost::gil::num_channels<View>::value;
	const bool onePass = _params._red && _params._green && _params._blue;
	this->progressBegin( this->_dstView.size() * (onePass ? 1 : nbChannels) );
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
	using namespace terry::algorithm;
	using namespace terry::color;
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
	    _params._blue )
	{
		// If all channel colors are inverted, which is the most used case
		// do it in one pass
		if( _params._alpha )
		{
			transform_pixels_progress(
				src,
				dst,
				transform_pixel_by_channel_t<terry::color::channel_invert_t>(),
				*this
				);
		}
		else
		{
			transform_pixels_progress(
				src,
				dst,
				pixel_invert_colors_t(),
				*this
				);
		}
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
				this->progressForward( dst.size() );
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
				this->progressForward( dst.size() );
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
				this->progressForward( dst.size() );
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
				this->progressForward( dst.size() );
			}
		}
	}

}

}
}
}
