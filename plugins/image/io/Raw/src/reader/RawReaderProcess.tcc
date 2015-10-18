#include "RawReaderDefinitions.hpp"
#include "RawReaderProcess.hpp"
#include "RawReaderPlugin.hpp"

#include <terry/globals.hpp>
#include <terry/point/ostream.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/assert.hpp>

namespace tuttle {
namespace plugin {
namespace raw {
namespace reader {

template<class View>
static int progressCallback( void* data, LibRaw_progress p, int iteration, int expected )
{
	typedef RawReaderProcess<View> PluginProcess;
	//PluginProcess* process = reinterpret_cast<PluginProcess*>( data );
	TUTTLE_LOG_DEBUG( "Callback: " << libraw_strprogress( p ) << "  pass " << iteration << " of " << expected );
	/*if( process->progressUpdate( iteration / (double)expected ) )
		return 1; // cancel processing immediately*/
	return 0; // can continue
}

//typedef void (*data_callback)(void *callback_data,const char *file, const int offset);
//void LibRaw::set_dataerror_handler(data_callback func, void *callback_data);

//typedef void (* memory_callback)(void *callback_data,const char *file, const char *where);
//void LibRaw::set_memerror_handler(memory_callback func,void *callback_data);

using namespace boost::gil;
namespace bfs = boost::filesystem;

template<class View>
RawReaderProcess<View>::RawReaderProcess( RawReaderPlugin& instance )
	: ImageGilProcessor<View>( instance, eImageOrientationFromTopToBottom )
	, _plugin( instance )
	, _p1( _rawProcessor.imgdata.idata )
	, _size( _rawProcessor.imgdata.sizes )
	, _color( _rawProcessor.imgdata.color )
	, _thumbnail( _rawProcessor.imgdata.thumbnail )
	, _p2( _rawProcessor.imgdata.other )
	, _out( _rawProcessor.imgdata.params )
{
	this->setNoMultiThreading();

	_rawProcessor.set_progress_handler( progressCallback<View>, reinterpret_cast<void*>( this ) );
	//	_rawProcessor.set_memerror_handler( xxxCallback, reinterpret_cast<void*>(this) );
	//	_rawProcessor.set_dataerror_handler( xxxCallback, reinterpret_cast<void*>(this) );
}

template<class View>
void RawReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.time );
}

template<class View>
void RawReaderProcess<View>::preProcess()
{
	// remove default implementation
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void RawReaderProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	// no tiles and no multithreading supported
	BOOST_ASSERT( procWindowRoW == this->_dstPixelRod );

	try
	{
		_out.output_bps = 16; // 16 bits
		_out.use_fuji_rotate = 0; // don't use rotation for cameras on a Fuji sensor

		_out.greybox[0] = _params._greyboxPoint.x;
		_out.greybox[1] = _params._greyboxPoint.y;
		_out.greybox[2] = _params._greyboxSize.x;
		_out.greybox[3] = _params._greyboxSize.y;

		_out.aber[0]   = _params._redAbber;
		_out.aber[2]   = _params._greenAbber;

		_out.gamm[0]   = _params._gammaPower;
		_out.gamm[1]   = _params._gammaToe;

		// brightness
		_out.bright    = _params._bright;
		_out.highlight  = _params._hightlight;
		_out.no_auto_bright = !_params._autoBright;

		// noise reduction
		_out.threshold = _params._threshold;
		_out.fbdd_noiserd = _params._fbddNoiseRd;

		// interpolation
#if LIBRAW_MAJOR_VERSION >= 0 && LIBRAW_MINOR_VERSION >= 16
		if( _params._interpolation == eInterpolationDisable )
		    _out.no_interpolation = 1; // disables interpolation step in LibRaw::dcraw_process() call.
		else
#endif
		    _out.user_qual = _params._interpolation;

		// interpolate colors
		_out.four_color_rgb = _params._fourColorRgb;
		_out.output_color = _params._outputColor;

		// exposure correction before demosaic
		_out.exp_shift  = _params._exposure;
		_out.exp_preser = _params._exposurePreserve;
		if( _out.exp_shift == 1.0 && _out.exp_preser == 0.0 )
		    _out.exp_correc = 0; // don't correct exposure
		else
		    _out.exp_correc = 1;
		
		// white balance
		_out.use_auto_wb = 0;
		_out.use_camera_wb = 0;
		switch( _params._whiteBalance )
		{
			case eAutoWb: _out.use_auto_wb = 1; break;
			case eCameraWb: _out.use_camera_wb = 1; break;
			case eManualWb: break;
			case e2500: break;
			case e2550: break;
			case e2650: break;
			case e2700: break;
			case e2800: break;
			case e2850: break;
			case e2950: break;
			case e3000: break;
			case e3100: break;
			case e3200: break;
			case e3300: break;
			case e3400: break;
			case e3600: break;
			case e3700: break;
			case e3800: break;
			case e4000: break;
			case e4200: break;
			case e4300: break;
			case e4500: break;
			case e4800: break;
			case e5000: break;
			case e5300: break;
			case e5600: break;
			case e5900: break;
			case e6300: break;
			case e6700: break;
			case e7100: break;
			case e7700: break;
			case e8300: break;
			case e9100: break;
			case e10000: break;
		}

		/*switch( _params._filtering )
		{
			case eFilteringAuto:
				_out.filtering_mode = LIBRAW_FILTERING_AUTOMATIC;
				break;
			case eFilteringNone:
				_out.filtering_mode = LIBRAW_FILTERING_NONE; // output RGBG ?
				break;
		}*/

		if( const int ret = _rawProcessor.open_file( _params._filepath.c_str() ) )
		{
			BOOST_THROW_EXCEPTION( exception::Unknown()
				<< exception::user() + "Cannot open file: " + libraw_strerror( ret )
				<< exception::filename( _params._filepath ) );
		}

		if( const int ret = _rawProcessor.unpack() )
		{
			BOOST_THROW_EXCEPTION( exception::Unknown()
				<< exception::user() + "Cannot unpack file: " + libraw_strerror( ret )
				<< exception::filename( _params._filepath ) );
		}
		
		// we should call dcraw_process before thumbnail extraction because for
		// some cameras (i.e. Kodak ones) white balance for thumbnal should be set
		// from main image settings

		// Data unpacking
		int ret = _rawProcessor.dcraw_process();
		
		if( LIBRAW_SUCCESS != ret )
		{
			if( LIBRAW_FATAL_ERROR( ret ) )
			{
				BOOST_THROW_EXCEPTION( exception::Unknown()
					<< exception::user() + "Cannot do postprocessing: " + libraw_strerror( ret )
					<< exception::filename( _params._filepath ) );
			}
			else
			{
				TUTTLE_LOG_ERROR( "Error on postprocessing (" << quotes(_params._filepath) << "): " << libraw_strerror( ret ) );
				TUTTLE_LOG_ERROR( "Try to continue..." );
			}
		}
		//		libraw_processed_image_t* image = _rawProcessor.dcraw_make_mem_image( &ret );
		//		if( ! image )
		//		{
		//			TUTTLE_LOG_ERROR( "Cannot unpack " << filepath << " to memory buffer: " << libraw_strerror( ret ) );
		//			return dst;
		//		}

		//		int ret = _rawProcessor.dcraw_document_mode_processing();
		//		if( LIBRAW_SUCCESS != ret )
		//		{
		//			TUTTLE_LOG_ERROR( "Cannot do document_mode_processing on " << filepath << " : " << libraw_strerror( ret ) );
		//			if( LIBRAW_FATAL_ERROR( ret ) )
		//				return dst;
		//		}

		// The metadata are accessible through data fields of the class
		TUTTLE_LOG_INFO( "Image size: " << _rawProcessor.imgdata.sizes.width << ", " << _rawProcessor.imgdata.sizes.height );

		//		TUTTLE_LOG_VAR2( image->width, image->height );
		TUTTLE_LOG_VAR2( TUTTLE_INFO, _rawProcessor.imgdata.sizes.width, _rawProcessor.imgdata.sizes.height );

		//		// And let us print its dump; the data are accessible through data fields of the class
		//		for( int i = 0;
		//			i < _rawProcessor.imgdata.sizes.iwidth *  _rawProcessor.imgdata.sizes.iheight;
		//			++i )
		//				TUTTLE_LOG( "i=" << i <<
		//					  " R=" << _rawProcessor.imgdata.image[i][0] <<
		//					  " G=" << _rawProcessor.imgdata.image[i][1] <<
		//					  " B=" << _rawProcessor.imgdata.image[i][2] <<
		//					  " G2=" << _rawProcessor.imgdata.image[i][3]
		//				);

		typedef boost::gil::rgba16c_view_t RawView;
		typedef RawView::value_type RawPixel;
		RawView imageView = interleaved_view( _size.width, _size.height, //image->width, image->height,
						      (const RawPixel*)( _rawProcessor.imgdata.image /*image->data*/ ),
						      _size.width /*image->width*/ * sizeof( RawPixel ) /*image->data_size*/ );

		View dst = this->_dstView;
		TUTTLE_LOG_VAR( TUTTLE_INFO, sizeof( RawPixel ) );
		TUTTLE_LOG_VAR2( TUTTLE_INFO, imageView.dimensions().x, imageView.dimensions().y );
		TUTTLE_LOG_VAR2( TUTTLE_INFO, dst.dimensions().x, dst.dimensions().y );
		copy_and_convert_pixels( imageView, dst );
		//		free( image );
		_rawProcessor.recycle();
	}
	catch( boost::exception& e )
	{
		e << exception::filename( _params._filepath );
		TUTTLE_LOG_ERROR( boost::diagnostic_information( e ) );
		//		throw;
	}
	catch( ... )
	{
		//		BOOST_THROW_EXCEPTION( exception::Unknown()
		//			<< exception::user( "Unable to write image")
		//			<< exception::filename(filepath) );
		TUTTLE_LOG_ERROR( boost::current_exception_diagnostic_information() );
	}
}

}
}
}
}
