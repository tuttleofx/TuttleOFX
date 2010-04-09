#include "PNGWriterDefinitions.hpp"
#include "PNGWriterPlugin.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/filesystem/fstream.hpp>
#include <cmath>
#include <vector>

namespace tuttle {
namespace plugin {
namespace png {
namespace writer {

template<class View>
PNGWriterProcess<View>::PNGWriterProcess( PNGWriterPlugin& instance )
	: ImageGilFilterProcessor<View>( instance ),
	_plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void PNGWriterProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 * @warning no multithread here !
 */
template<class View>
void PNGWriterProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	BOOST_ASSERT( procWindowRoW == this->_srcPixelRod );
	using namespace boost::gil;
	try
	{
		writeImage( this->_srcView, _plugin.getParams(this->_renderArgs.time)._filepath );
		copy_pixels( this->_srcView, this->_dstView );
	}
	catch( tuttle::plugin::PluginException& e )
	{
		COUT_EXCEPTION( e );
	}
}

/**
 * 
 */
template<class View>
void PNGWriterProcess<View>::writeImage( View& src, const std::string& filepath ) throw( tuttle::plugin::PluginException )
{
	using namespace boost::gil;
	View flippedView = flipped_up_down_view( src );
	typedef typename boost::gil::channel_type<typename View::value_type>::type ValueType;
	// Always drop alpha channel
	typedef typename boost::gil::pixel<ValueType, rgb_layout_t > PixelType;

	png_write_view( filepath, color_converted_view<PixelType>( flippedView ) );
}

template<>
void PNGWriterProcess<gray32f_view_t>::writeImage( gray32f_view_t& src, const std::string& filepath ) throw( tuttle::plugin::PluginException )
{
	using namespace boost::gil;
	gray32f_view_t flippedView = flipped_up_down_view( src );
	png_write_view( filepath, color_converted_view<gray16_pixel_t>( flippedView ) );
}

template<>
void PNGWriterProcess<rgb32f_view_t>::writeImage( rgb32f_view_t& src, const std::string& filepath ) throw( tuttle::plugin::PluginException )
{
	using namespace boost::gil;
	rgb32f_view_t flippedView = flipped_up_down_view( src );
	png_write_view( filepath, color_converted_view<rgb16_pixel_t>( flippedView ) );
}

template<>
void PNGWriterProcess<rgba32f_view_t>::writeImage( rgba32f_view_t& src, const std::string& filepath ) throw( tuttle::plugin::PluginException )
{
	using namespace boost::gil;
	rgba32f_view_t flippedView = flipped_up_down_view( src );
	png_write_view( filepath, color_converted_view<rgb16_pixel_t>( flippedView ) );
}

}
}
}
}
