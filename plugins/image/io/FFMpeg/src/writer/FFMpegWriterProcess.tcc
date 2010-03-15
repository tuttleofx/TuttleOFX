namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

template<class View>
FFMpegProcess<View>::FFMpegProcess( FFMpegWriterPlugin &instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
{
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void FFMpegProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	using namespace boost::gil;
	rgb8_image_t img(this->_srcView.dimensions());
	rgb8_view_t vw(view(img));
	// Convert pixels in PIX_FMT_RGB24
	copy_and_convert_pixels(this->_srcView, vw);
	// Convert pixels to destination
	copy_and_convert_pixels(this->_srcView, this->_dstView);
	uint8_t* pixels = (uint8_t*)boost::gil::interleaved_view_get_raw_data( this->_srcView );
	// Execute writing
	_plugin.getWriter().execute( pixels, this->_srcView.width(), this->_srcView.height(), PIX_FMT_RGB24);
}

}
}
}
}
