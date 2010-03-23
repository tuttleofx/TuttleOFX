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
	COUT_INFOS;
}


template<class View>
void FFMpegWriterProcess<View>::preProcess()
{
	COUT_INFOS;
	ImageGilProcessor<View>::preProcess();
	/*
	if( !boost::filesystem::exists( _params._filepath ) )
	{
		return;
	}
	*/
	_writer.filename( _params._filepath );
	COUT_VAR( _params._format );
	_writer.setFormat( _params._format );
	COUT_VAR( _params._codec );
	_writer.setCodec( _params._codec );
}

template<class View>
void FFMpegWriterProcess<View>::postProcess()
{
	ImageGilProcessor<View>::postProcess();
	_writer.finish();
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

	_writer.width( this->_srcView.width() );
	_writer.height( this->_srcView.height() );

	rgb8_image_t img( this->_srcView.dimensions() );
	rgb8_view_t vw( view(img) );
	// Convert pixels in PIX_FMT_RGB24
	copy_and_convert_pixels( this->_srcView, vw );
	// Convert pixels to destination
	copy_and_convert_pixels( this->_srcView, this->_dstView );
	uint8_t* pixels = (uint8_t*)boost::gil::interleaved_view_get_raw_data( this->_srcView );
	// Execute writing
	_writer.execute( pixels, this->_srcView.width(), this->_srcView.height(), PIX_FMT_RGB24 );
}

}
}
}
}
