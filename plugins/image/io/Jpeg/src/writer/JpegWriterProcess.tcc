#include "JpegWriterDefinitions.hpp"
#include "JpegWriterPlugin.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/filesystem/fstream.hpp>
#include <cmath>
#include <vector>

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace writer {

template<class View>
JpegWriterProcess<View>::JpegWriterProcess( JpegWriterPlugin& instance )
	: ImageGilFilterProcessor<View>( instance ),
	_plugin( instance )
{
	this->setNoMultiThreading();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 * @warning no multithread here !
 */
template<class View>
void JpegWriterProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	BOOST_ASSERT( procWindowRoW == this->_srcPixelRod );
	using namespace boost::gil;
	try
	{
		JpegWriterProcessParams params = _plugin.getProcessParams(this->_renderArgs.time);
		writeImage<bits8>( this->_srcView, params._filepath );
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
template<class Bits>
void JpegWriterProcess<View>::writeImage( View& src, const std::string& filepath )
{
	using namespace boost::gil;

	JpegWriterProcessParams params = _plugin.getProcessParams( this->_renderArgs.time );

//	if( params._premult )
//	{
		typedef pixel<Bits, rgb_layout_t> OutPixelType;
		jpeg_write_view( filepath, flipped_up_down_view( color_converted_view<OutPixelType>( clamp<OutPixelType>( src ) ) ), params._quality );
//	}
//	else
//	{
//		typedef pixel<Bits, layout<typename color_space_type<View>::type> > OutPixelType;
//		jpeg_write_view( filepath, flipped_up_down_view( color_converted_view<OutPixelType>( clamp<OutPixelType>( src ) ) ) );
//	}
}

}
}
}
}
