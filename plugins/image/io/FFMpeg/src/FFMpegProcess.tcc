namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace reader {

template<class View>
FFMpegProcess<View>::FFMpegProcess( FFMpegPlugin &instance )
: ImageGilFilterProcessor<View>( instance )
, _plugin( instance )
{
}

template<class View>
void FFMpegProcess<View>::setup( const OFX::RenderArguments& args )
{
    /*
	std::string sFilepath;
	// Fetch output image
	_filepath->getValue( sFilepath );
	if( ! bfs::exists( sFilepath ) )
		throw tuttle::plugin::PluginException( "Unable to open : " + sFilepath );
	_plugin.getDpxImg().read( sFilepath, true );

	point2<ptrdiff_t> imageDims( _plugin.getDpxImg().width(),
								 _plugin.getDpxImg().height() );

	double par       = _plugin.getDstClip()->getPixelAspectRatio();
	OfxRectD reqRect = { 0, 0, imageDims.x * par, imageDims.y };
	this->_dst.reset( _plugin.getDstClip()->fetchImage( args.time, reqRect ) );
	OfxRectI bounds = this->_dst->getBounds();
	if( !this->_dst.get() )
		throw( tuttle::plugin::ImageNotReadyException() );

	// Build destination view
	this->_dstView = this->getView( this->_dst.get(),
	 								_plugin.getDstClip()->getPixelRod(args.time) );
     */
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void FFMpegProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
/*
    using namespace boost::gil;

    View src = subimage_view( this->_srcView, procWindow.x1, procWindow.y1,
                                              procWindow.x2 - procWindow.x1,
                                              procWindow.y2 - procWindow.y1 );
    View dst = subimage_view( this->_dstView, procWindow.x1, procWindow.y1,
                                              procWindow.x2 - procWindow.x1,
                                              procWindow.y2 - procWindow.y1 );
    copy_pixels( src, dst );
*/
}

}
}
}
}
