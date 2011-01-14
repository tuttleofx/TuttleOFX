#include "RawReaderDefinitions.hpp"
#include "RawReaderProcess.hpp"
#include "RawReaderPlugin.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
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
	PluginProcess* process = reinterpret_cast<PluginProcess*>( data );
	TUTTLE_COUT( "Callback: " << libraw_strprogress( p ) << "  pass " << iteration << " of " << expected );
	if( process->progressForward( iteration ) )
		return 1; // cancel processing immediately
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
	: ImageGilProcessor<View>( instance )
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
		_out.output_bps = 16;
		//		_out.user_flip  = 1;
		//		_out.document_mode = 2;
		//		_out.half_size  = 1;
		//		_out.four_color_rgb = 1;
		_out.gamm[0] = _out.gamm[1] =  _out.no_auto_bright    = 1;
		switch( _params._filtering )
		{
			case eFilteringAuto:
				_out.filtering_mode = LIBRAW_FILTERING_AUTOMATIC;
				break;
			case eFilteringNone:
				_out.filtering_mode = LIBRAW_FILTERING_NONE; // output RGBG ?
				break;
		}

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
				TUTTLE_COUT_ERROR( "Error on postprocessing (" << quotes(_params._filepath) << "): " << libraw_strerror( ret ) );
				TUTTLE_COUT_ERROR( "Try to continue..." );
			}
		}
		//		libraw_processed_image_t* image = _rawProcessor.dcraw_make_mem_image( &ret );
		//		if( ! image )
		//		{
		//			TUTTLE_COUT_ERROR( "Cannot unpack " << filepath << " to memory buffer: " << libraw_strerror( ret ) );
		//			return dst;
		//		}

		//		int ret = _rawProcessor.dcraw_document_mode_processing();
		//		if( LIBRAW_SUCCESS != ret )
		//		{
		//			TUTTLE_COUT_ERROR( "Cannot do document_mode_processing on " << filepath << " : " << libraw_strerror( ret ) );
		//			if( LIBRAW_FATAL_ERROR( ret ) )
		//				return dst;
		//		}

		// The metadata are accessible through data fields of the class
		TUTTLE_COUT( "Image size: " << _rawProcessor.imgdata.sizes.width << ", " << _rawProcessor.imgdata.sizes.height );

		//		TUTTLE_COUT_VAR2( image->width, image->height );
		TUTTLE_COUT_VAR2( _rawProcessor.imgdata.sizes.width, _rawProcessor.imgdata.sizes.height );

		//		// And let us print its dump; the data are accessible through data fields of the class
		//		for( int i = 0;
		//			i < _rawProcessor.imgdata.sizes.iwidth *  _rawProcessor.imgdata.sizes.iheight;
		//			++i )
		//				TUTTLE_COUT( "i=" << i <<
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
		TUTTLE_COUT_VAR( sizeof( RawPixel ) );
		TUTTLE_COUT_VAR( imageView.dimensions() );
		TUTTLE_COUT_VAR( dst.dimensions() );
		if( _params._flip )
		{
			dst = flipped_up_down_view( this->_dstView );
		}
		copy_and_convert_pixels( imageView, dst );
		//		free( image );
		_rawProcessor.recycle();
	}
	catch( boost::exception& e )
	{
		e << exception::filename( _params._filepath );
		TUTTLE_COUT_ERROR( boost::diagnostic_information( e ) );
		//		throw;
	}
	catch(... )
	{
		//		BOOST_THROW_EXCEPTION( exception::Unknown()
		//			<< exception::user( "Unable to write image")
		//			<< exception::filename(filepath) );
		TUTTLE_COUT_ERROR( boost::current_exception_diagnostic_information() );
	}
}

}
}
}
}
