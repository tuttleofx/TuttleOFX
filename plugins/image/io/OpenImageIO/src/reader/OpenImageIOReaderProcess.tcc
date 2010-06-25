#include "OpenImageIOReaderDefinitions.hpp"
#include "OpenImageIOReaderProcess.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <OpenImageIO/imageio.h>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/assert.hpp>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace reader {

using namespace boost::gil;
namespace bfs = boost::filesystem;


template<class View>
OpenImageIOReaderProcess<View>::OpenImageIOReaderProcess( OpenImageIOReaderPlugin& instance )
	: ImageGilProcessor<View>( instance ),
	_plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void OpenImageIOReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	OpenImageIOReaderProcessParams params = _plugin.getProcessParams(args.time);
	if( ! bfs::exists( params._filepath ) )
	{
		throw( OFX::Exception::Suite(kOfxStatFailed, std::string("Unable to open : ") + params._filepath ) );
	}

	ImageGilProcessor<View>::setup( args );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void OpenImageIOReaderProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	// no tiles and no multithreading supported
	BOOST_ASSERT( procWindowRoW == this->_dstPixelRod );
	readImage( this->_dstView, _plugin.getProcessParams(this->_renderArgs.time)._filepath );
}

/**
 */
template<class View>
View& OpenImageIOReaderProcess<View>::readImage( View& dst, const std::string& filepath ) throw( PluginException )
{
	/*
	any_image_t anyImg;
	try
	{
		openImageIO_read_image( filepath, anyImg );
	}
	catch( PluginException& e )
	{
		COUT_EXCEPTION( e );
		return dst;
	}
	copy_and_convert_pixels( subimage_view( flipped_up_down_view( view( anyImg ) ), 0, 0, dst.width(), dst.height() ), dst );
	*/
	return dst;
}

}
}
}
}
