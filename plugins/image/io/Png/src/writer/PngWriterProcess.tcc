#include "PngWriterDefinitions.hpp"
#include "PngWriterPlugin.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/filesystem/fstream.hpp>
#include <cmath>
#include <vector>

namespace tuttle {
namespace plugin {
namespace png {
namespace writer {

template<class View>
PngWriterProcess<View>::PngWriterProcess( PngWriterPlugin& instance )
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
void PngWriterProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	BOOST_ASSERT( procWindowRoW == this->_srcPixelRod );
	using namespace boost::gil;
	PngWriterProcessParams params = _plugin.getProcessParams(this->_renderArgs.time);
	try
	{
		switch(params._bitDepth)
		{
			case 8:
				writeImage<bits8>( this->_srcView, params._filepath );
				break;
			case 16:
				writeImage<bits16>( this->_srcView, params._filepath );
				break;
			default:
				BOOST_THROW_EXCEPTION( PluginException( "Unrecognized bit depth" ) );
				break;
		}
	}
	catch( PluginException& e )
	{
		throw;
	}
	catch( ... )
	{
		BOOST_THROW_EXCEPTION( PluginException( "Unable to write image: \"" + params._filepath + "\"" ) );
	}
	copy_pixels( this->_srcView, this->_dstView );
}

/**
 * 
 */
template<class View>
template<class Bits>
void PngWriterProcess<View>::writeImage( View& src, const std::string& filepath )
{
	using namespace boost::gil;

	if ( _plugin.getProcessParams(this->_renderArgs.time)._outputRGB )
	{
		typedef pixel<Bits, rgb_layout_t> OutPixelType;
		png_write_view( filepath, flipped_up_down_view( color_converted_view<OutPixelType>( clamp<OutPixelType>( src ) ) ) );
	}
	else
	{
		typedef pixel<Bits, layout<typename color_space_type<View>::type> > OutPixelType;
		png_write_view( filepath, flipped_up_down_view( color_converted_view<OutPixelType>( clamp<OutPixelType>( src ) ) ) );
	}
}

}
}
}
}
