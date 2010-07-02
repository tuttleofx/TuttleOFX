#include "PngReaderDefinitions.hpp"
#include "PngReaderProcess.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/filesystem/fstream.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/assert.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace reader {

using namespace boost::gil;
namespace bfs = boost::filesystem;

typedef any_image < boost::mpl::vector
                    < rgba8_image_t, rgba16_image_t, rgba32f_image_t,
                      rgb8_image_t,  rgb16_image_t,  rgb32f_image_t >
                    > any_image_t;

template<class View>
PngReaderProcess<View>::PngReaderProcess( PngReaderPlugin& instance )
	: ImageGilProcessor<View>( instance ),
	_plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void PngReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	PngReaderProcessParams params = _plugin.getProcessParams(args.time);
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
void PngReaderProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	// no tiles and no multithreading supported
	BOOST_ASSERT( procWindowRoW == this->_dstPixelRod );
	readImage( this->_dstView, _plugin.getProcessParams(this->_renderArgs.time)._filepath );
}

/**
 */
template<class View>
View& PngReaderProcess<View>::readImage( View& dst, const std::string& filepath ) throw( PluginException )
{
	any_image_t anyImg;
	try
	{
		png_read_image( filepath, anyImg );
	}
	catch( PluginException& e )
	{
		COUT_EXCEPTION( e );
		return dst;
	}
	copy_and_convert_pixels( subimage_view( flipped_up_down_view( view( anyImg ) ), 0, 0, dst.width(), dst.height() ), dst );
	return dst;
}

}
}
}
}
