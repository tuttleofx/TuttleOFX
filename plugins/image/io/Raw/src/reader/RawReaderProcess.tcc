#include "RawReaderDefinitions.hpp"
#include "RawReaderProcess.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/assert.hpp>

namespace tuttle {
namespace plugin {
namespace raw {
namespace reader {

using namespace boost::gil;
namespace bfs = boost::filesystem;

//typedef boost::gil::ny_image < boost::mpl::vector
//                    < rgba8_image_t, rgba16_image_t, rgba32f_image_t,
//                      rgb8_image_t,  rgb16_image_t,  rgb32f_image_t >
//                    > any_image_t;

template<class View>
RawReaderProcess<View>::RawReaderProcess( RawReaderPlugin& instance )
	: ImageGilProcessor<View>( instance ),
	_plugin( instance ),
	_p1( _rawProcessor.imgdata.idata ),
	_size( _rawProcessor.imgdata.sizes ),
	_color( _rawProcessor.imgdata.color ),
	_thumbnail( _rawProcessor.imgdata.thumbnail ),
	_p2( _rawProcessor.imgdata.other ),
	_out( _rawProcessor.imgdata.params )
{
	this->setNoMultiThreading();
}

template<class View>
void RawReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	RawReaderProcessParams params = _plugin.getProcessParams( args.time );

	if( !bfs::exists( params._filepath ) )
	{
		throw( OFX::Exception::Suite( kOfxStatFailed, std::string( "Unable to open : " ) + params._filepath ) );
	}

	ImageGilProcessor<View>::setup( args );
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
	readImage( this->_dstView, _plugin.getProcessParams( this->_renderArgs.time )._filepath );
}

/**
 */
template<class View>
View& RawReaderProcess<View>::readImage( View& dst, const std::string& filepath ) throw( PluginException )
{
	try
	{
		_out.output_bps = 16;
		_out.gamm[0]    = _out.gamm[1] =  _out.no_auto_bright    = 1;
//		_out.filtering_mode = LIBRAW_FILTERING_NONE; // output RGBG
		
		if( const int ret = _rawProcessor.open_file( filepath.c_str() ) )
		{
			COUT_ERROR( "Cannot open " << filepath << " : " << libraw_strerror( ret ) );
			return dst;
		}

		if( const int ret = _rawProcessor.unpack() )
		{
			COUT_ERROR( "Cannot unpack " << filepath << " : " << libraw_strerror( ret ) );
			return dst;
		}

		// we should call dcraw_process before thumbnail extraction because for
		// some cameras (i.e. Kodak ones) white balance for thumbnal should be set
		// from main image settings

		int ret = _rawProcessor.dcraw_process();

		if( LIBRAW_SUCCESS != ret )
		{
			COUT_ERROR( "Cannot do postpocessing on " << filepath << " : " << libraw_strerror( ret ) );
			if( LIBRAW_FATAL_ERROR( ret ) )
				return dst;
		}
		libraw_processed_image_t* image = _rawProcessor.dcraw_make_mem_image( &ret );
		if( ! image )
		{
			COUT_ERROR( "Cannot unpack " << filepath << " to memory buffer: " << libraw_strerror( ret ) );
			return dst;
		}

		using namespace boost::gil;
		rgb16c_view_t imageView = interleaved_view( image->width, image->height,
							                  (const rgb16c_pixel_t*)( image->data ),
							                  image->width * sizeof( rgb16c_pixel_t ) /*image->data_size*/ );
		COUT_VAR( imageView.dimensions() );
		COUT_VAR( dst.dimensions() );
		copy_and_convert_pixels( imageView, dst );
		free( image );
		_rawProcessor.recycle();
	}
	catch( PluginException& e )
	{
		COUT_EXCEPTION( e );
		return dst;
	}
	return dst;
}


}
}
}
}
