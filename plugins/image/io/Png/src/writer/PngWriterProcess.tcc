#include "PngWriterDefinitions.hpp"
#include "PngWriterPlugin.hpp"

#include <terry/globals.hpp>
#include <terry/clamp.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace writer {

template<class View>
PngWriterProcess<View>::PngWriterProcess( PngWriterPlugin& instance )
	: ImageGilFilterProcessor<View>( instance, eImageOrientationFromTopToBottom )
	, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void PngWriterProcess<View>::setup( const OFX::RenderArguments& args )
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
void PngWriterProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	BOOST_ASSERT( procWindowRoW == this->_srcPixelRod );
	using namespace boost::gil;

	View srcView = this->_srcView;

	try
	{
		switch( _params._bitDepth )
		{
			case eTuttlePluginBitDepth8:
				writeImage<bits8>( srcView );
				break;
			case eTuttlePluginBitDepth16:
				writeImage<bits16>( srcView );
				break;
			default:
				BOOST_THROW_EXCEPTION( exception::ImageFormat()
				    << exception::user( "PNG Writer: Unrecognized bit depth" ) );
				break;
		}
	}
	catch( exception::Common& e )
	{
		e << exception::filename( _params._filepath );
		throw;
	}
	catch(... )
	{
		BOOST_THROW_EXCEPTION( exception::Unknown()
			<< exception::user( "Unable to write image")
			<< exception::dev( boost::current_exception_diagnostic_information() )
			<< exception::filename( _params._filepath ) );
	}
	copy_pixels( this->_srcView, this->_dstView ); /// @todo ?
}

/**
 *
 */
template<class View>
template<class Bits>
void PngWriterProcess<View>::writeImage( View& src )
{
	using namespace boost::gil;
	using namespace terry;

	switch( _params._components )
	{
		case eTuttlePluginComponentsRGBA:
		{
			typedef pixel<Bits, layout<typename color_space_type<View>::type> > OutPixelType;
			png_write_view( _params._filepath, color_converted_view<OutPixelType>( clamp_view( src ) ) );
			break;
		}
		case eTuttlePluginComponentsRGB:
		{
			typedef pixel<Bits, rgb_layout_t> OutPixelType;
			png_write_view( _params._filepath, color_converted_view<OutPixelType>( clamp_view( src ) ) );
			break;
		}
		case eTuttlePluginComponentsGray:
		{
			typedef pixel<Bits, gray_layout_t> OutPixelType;
			png_write_view( _params._filepath, color_converted_view<OutPixelType>( clamp_view( src ) ) );
			break;
		}
		default:
			BOOST_THROW_EXCEPTION( exception::ImageFormat()
			    << exception::user( "PNG Writer: Unrecognized component choice." ) );
			break;
	}
}

}
}
}
}
