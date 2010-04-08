namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace reader {

template<class View>
FFMpegReaderProcess<View>::FFMpegReaderProcess( FFMpegReaderPlugin &instance )
: ImageGilProcessor<View>( instance )
, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void FFMpegReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	std::string sFilepath;
	// Fetch output image
	if( _plugin.getReader().read((int)args.time) )
		throw( tuttle::plugin::ExceptionAbort() );

	ImageGilProcessor<View>::setup( args );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void FFMpegReaderProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	BOOST_ASSERT( procWindowRoW == this->_dstPixelRod );
	
	rgb8c_view_t ffmpegSrcView =
			interleaved_view( _plugin.getReader().width(), _plugin.getReader().height(),
							  (const rgb8c_pixel_t*)( _plugin.getReader().data() ),
							  _plugin.getReader().width() * 3 );

	copy_and_convert_pixels(ffmpegSrcView, flipped_up_down_view( this->_dstView ) );
}

}
}
}
}
