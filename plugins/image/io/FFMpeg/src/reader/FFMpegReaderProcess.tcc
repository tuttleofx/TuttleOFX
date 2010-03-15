namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace reader {

template<class View>
FFMpegReaderProcess<View>::FFMpegReaderProcess( FFMpegReaderPlugin &instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
{
}

template<class View>
void FFMpegReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	std::string sFilepath;
	// Fetch output image
	if ( !_plugin.getReader().read((int)args.time) )
	{
		boost::gil::point2<ptrdiff_t> imageDims( _plugin.getReader().width(),
									 _plugin.getReader().height() );

		double par       = _plugin.getDstClip()->getPixelAspectRatio();
		OfxRectD reqRect = { 0, 0, imageDims.x * par, imageDims.y };
		this->_dst.reset( _plugin.getDstClip()->fetchImage( args.time, reqRect ) );
		OfxRectI bounds = this->_dst->getBounds();
		if( !this->_dst.get() )
			throw( tuttle::plugin::ImageNotReadyException() );

		// Build destination view
		this->_dstView = this->getView( this->_dst.get(),
										_plugin.getDstClip()->getPixelRod(args.time) );
	}
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void FFMpegReaderProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	using namespace boost::gil;
	rgb8c_view_t ffmpegSrcView =
			interleaved_view( _plugin.getReader().width(), _plugin.getReader().height(),
							  (const rgb8c_pixel_t*)( _plugin.getReader().data() ),
							  _plugin.getReader().width() * 3 );

	copy_and_convert_pixels(ffmpegSrcView, this->_dstView);
}

}
}
}
}
