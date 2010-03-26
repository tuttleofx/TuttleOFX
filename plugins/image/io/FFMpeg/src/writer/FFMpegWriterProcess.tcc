#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterProcess.hpp"
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

template<class View>
FFMpegWriterProcess<View>::FFMpegWriterProcess( FFMpegWriterPlugin &instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
, _params( _plugin.getProcessParams() )
{
	this->setNoMultiThreading();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void FFMpegWriterProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	using namespace boost::gil;
	J2KWriter & writer = _plugin.getWriter();
	writer.width( this->_srcView.width() );
	writer.height( this->_srcView.height() );

	rgb8_image_t img( this->_srcView.dimensions() );
	rgb8_view_t vw( view(img) );
	// Convert pixels in PIX_FMT_RGB24
	copy_and_convert_pixels( this->_srcView, flipped_up_down_view( vw ) );
	// Convert pixels to destination
	copy_and_convert_pixels( this->_srcView, this->_dstView );
	uint8_t* pixels = (uint8_t*)boost::gil::interleaved_view_get_raw_data( vw );
	// Execute writing
	writer.setBitrate( _params._bitrate );
	writer.execute( pixels, this->_srcView.width(), this->_srcView.height(), PIX_FMT_RGB24 );
}

}
}
}
}
