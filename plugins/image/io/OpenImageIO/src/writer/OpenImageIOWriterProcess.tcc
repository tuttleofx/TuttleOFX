#include "OpenImageIOWriterDefinitions.hpp"
#include "OpenImageIOWriterPlugin.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <OpenImageIO/imageio.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>
#include <cmath>
#include <vector>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace writer {

template<class View>
OpenImageIOWriterProcess<View>::OpenImageIOWriterProcess( OpenImageIOWriterPlugin& instance )
	: ImageGilFilterProcessor<View>( instance ),
	_plugin( instance )
{
	this->setNoMultiThreading();
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 * @warning no multithread here !
 */
template<class View>
void OpenImageIOWriterProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	BOOST_ASSERT( procWindowRoW == this->_srcPixelRod );
	using namespace boost::gil;
	try
	{
		OpenImageIOWriterProcessParams params = _plugin.getParams(this->_renderArgs.time);
		switch(params._bitDepth)
		{
			case 8:
				writeImage<bits8>( this->_srcView, params._filepath );
				break;
			case 16:
				writeImage<bits16>( this->_srcView, params._filepath );
				break;

		}
		copy_pixels( this->_srcView, this->_dstView );
	}
	catch( tuttle::plugin::PluginException& e )
	{
		COUT_EXCEPTION( e );
	}
}

/**
 * 
 */
template<class View>
template<class Bits>
void OpenImageIOWriterProcess<View>::writeImage( View& src, const std::string& filepath )
{
	using namespace boost::gil;
	/*
	if ( _plugin.getParams(this->_renderArgs.time)._outputRGB )
	{
		typedef pixel<Bits, rgb_layout_t> OutPixelType;
		openImageIO_write_view( filepath, flipped_up_down_view( color_converted_view<OutPixelType>( clamp<OutPixelType>( src ) ) ) );
	}
	else
	{
		typedef pixel<Bits, layout<typename color_space_type<View>::type> > OutPixelType;
		openImageIO_write_view( filepath, flipped_up_down_view( color_converted_view<OutPixelType>( clamp<OutPixelType>( src ) ) ) );
	}
	*/
}

}
}
}
}
