#include "JpegWriterDefinitions.hpp"
#include "JpegWriterPlugin.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/clamp.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/gil/extension/io/jpeg_io.hpp>
#include <boost/filesystem/fstream.hpp>

#include <cmath>
#include <vector>

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace writer {

template<class View>
JpegWriterProcess<View>::JpegWriterProcess( JpegWriterPlugin& instance )
	: ImageGilFilterProcessor<View>( instance )
	, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void JpegWriterProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );

	_params = _plugin.getProcessParams( args.time );
}


/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 * @warning no multithread here !
 */
template<class View>
void JpegWriterProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	BOOST_ASSERT( procWindowRoW == this->_srcPixelRod );
	using namespace boost::gil;

	View srcView = this->_srcView;
	if( _params._flip )
	{
		srcView = flipped_up_down_view( srcView );
	}

	try
	{
		writeImage<bits8>( srcView );
	}
	catch( exception::Common& e )
	{
		e << exception::filename( _params._filepath );
		TUTTLE_COUT_ERROR( boost::diagnostic_information( e ) );
		//		throw;
	}
	catch(... )
	{
		//		BOOST_THROW_EXCEPTION( exception::Unknown()
		//			<< exception::user( "Unable to write image")
		//			<< exception::filename(params._filepath) );
		TUTTLE_COUT_ERROR( boost::current_exception_diagnostic_information() );
	}
	copy_pixels( this->_srcView, this->_dstView );
}

/**
 *
 */
template<class View>
template<class Bits>
void JpegWriterProcess<View>::writeImage( View& src )
{
	using namespace boost::gil;

	JpegWriterProcessParams params = _plugin.getProcessParams( this->_renderArgs.time );

	//	if( params._premult )
	//	{
	typedef pixel<Bits, rgb_layout_t> OutPixelType;
	jpeg_write_view( params._filepath, flipped_up_down_view( color_converted_view<OutPixelType>( clamp_view( src ) ) ), params._quality );
	//	}
	//	else
	//	{
	//		typedef pixel<Bits, layout<typename color_space_type<View>::type> > OutPixelType;
	//		jpeg_write_view( params._filepath, flipped_up_down_view( color_converted_view<OutPixelType>( clamp_view( src ) ) ) );
	//	}
}

}
}
}
}
