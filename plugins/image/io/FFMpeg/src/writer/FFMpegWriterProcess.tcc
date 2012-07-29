#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterProcess.hpp"
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

template<class View>
FFMpegWriterProcess<View>::FFMpegWriterProcess( FFMpegWriterPlugin& instance )
	: ImageGilFilterProcessor<View>( instance, eImageOrientationFromTopToBottom )
	, _plugin( instance )
	, _params( _plugin.getProcessParams() )
{
	this->setNoMultiThreading();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void FFMpegWriterProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	BOOST_ASSERT( procWindowRoW == this->_dstPixelRod );

	_plugin._writer.width ( this->_srcView.width () );
	_plugin._writer.height( this->_srcView.height() );

	rgb8_image_t img ( this->_srcView.dimensions() );
	rgb8_view_t  vw  ( view( img ) );

	// Convert pixels in PIX_FMT_RGB24
	copy_and_convert_pixels( this->_srcView, vw );

	// Convert pixels to destination
	copy_and_convert_pixels( this->_srcView, this->_dstView );
	uint8_t* pixels = (uint8_t*)boost::gil::interleaved_view_get_raw_data( vw );

	// Execute writing
	_plugin._writer.execute( pixels, this->_srcView.width(), this->_srcView.height(), PIX_FMT_RGB24 );
}

}
}
}
}
