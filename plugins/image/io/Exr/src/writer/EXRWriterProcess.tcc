#include "EXRWriterDefinitions.hpp"
#include "EXRWriterPlugin.hpp"


#include <terry/globals.hpp>
#include <terry/openexr/half.hpp>

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/cstdint.hpp>
#include <boost/assert.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

using namespace boost::gil;

template<class View>
EXRWriterProcess<View>::EXRWriterProcess( EXRWriterPlugin& instance )
	: ImageGilFilterProcessor<View>( instance, eImageOrientationFromTopToBottom )
	, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void EXRWriterProcess<View>::setup( const OFX::RenderArguments& args )
{
	using namespace boost::gil;
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.time );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void EXRWriterProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	// no tiles and no multithreading supported
	BOOST_ASSERT( ( procWindowRoW == this->_dstPixelRod ) );
	BOOST_ASSERT( ( this->_srcPixelRod == this->_dstPixelRod ) );

	View src = this->_srcView;

	try
	{
		switch( _params._bitDepth )
		{
			case eTuttlePluginBitDepth16f:
			{
				switch( _params._componentsType )
				{
					case eTuttlePluginComponentsGray:
					{
						writeGrayImage<gray16h_pixel_t>( src, _params._filepath, Imf::HALF );
						break;
					}
					case eTuttlePluginComponentsRGB:
					{
						writeImage<rgb16h_pixel_t>( src, _params._filepath, Imf::HALF );
						break;
					}
					case eTuttlePluginComponentsRGBA:
					{
						writeImage<rgba16h_pixel_t>( src, _params._filepath, Imf::HALF );
						break;
					}
					default:
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "Exr Writer: components not supported" ) );
				}
				break;
			}
			case eTuttlePluginBitDepth32f:
			{
				switch( _params._componentsType )
				{
					case eTuttlePluginComponentsGray:
					{
						writeGrayImage<gray32f_pixel_t>(src, _params._filepath, Imf::FLOAT);
						break;
					}
					case eTuttlePluginComponentsRGB:
					{
						writeImage<rgb32f_pixel_t>( src, _params._filepath, Imf::FLOAT );
						break;
					}
					case eTuttlePluginComponentsRGBA:
					{
						writeImage<rgba32f_pixel_t>( src, _params._filepath, Imf::FLOAT );
						break;
					}
					default:
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "Exr Writer: components not supported" ) );
				}
				break;
			}
			case eTuttlePluginBitDepth32:
			{
				switch( _params._componentsType )
				{
					case eTuttlePluginComponentsGray:
					{
						writeGrayImage<gray32_pixel_t>(src, _params._filepath, Imf::UINT);
						break;
					}
					case eTuttlePluginComponentsRGB:
					{
						writeImage<rgb32_pixel_t>( src, _params._filepath, Imf::UINT );
						break;
					}
					case eTuttlePluginComponentsRGBA:
					{
						writeImage<rgba32_pixel_t>( src, _params._filepath, Imf::UINT );
						break;
					}
					default:
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "Exr Writer: components not supported" ) );
				}
				break;
			}
			default:
				BOOST_THROW_EXCEPTION( exception::Unsupported()
				    << exception::user( "Exr Writer: bit depth not supported" ) );
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
			<< exception::user( "Unable to write image" )
			<< exception::dev( boost::current_exception_diagnostic_information() )
			<< exception::filename( _params._filepath ) );
	}
	// @todo: This is sometimes not neccessary... Checkbox it.
	copy_and_convert_pixels( this->_srcView, this->_dstView );
}

template<class View>
template<class WPixel>
void EXRWriterProcess<View>::writeGrayImage( View& src, std::string& filepath, Imf::PixelType pixType )
{
	using namespace terry;

	std::size_t bitsTypeSize = 0;

	typedef image<WPixel, false> image_t;
	typedef typename image_t::view_t view_t;
	image_t img( src.width(), src.height() );
	view_t  dvw( view( img ) );
	copy_and_convert_pixels( src, dvw );
	Imf::Header header( src.width(), src.height() );
	switch( pixType )
	{
		case Imf::HALF:
			bitsTypeSize = sizeof( half );
			break;
		case Imf::FLOAT:
			bitsTypeSize = sizeof( float );
			break;
		case Imf::UINT:
			bitsTypeSize = sizeof( boost::uint32_t );
			break;
		default:
			break;
	}

	switch( dvw.num_channels() )
	{
		// Gray
		case 1:
		{
			header.channels().insert( "Y", Imf::Channel( pixType ) );
			break;
		}
		// RGB
		// RGBA
		default:
			BOOST_THROW_EXCEPTION( exception::ImageFormat()
			    << exception::user( "ExrWriter: incompatible image type" ) );
			break;
	}

	Imf::OutputFile file( filepath.c_str(), header );
	Imf::FrameBuffer frameBuffer;

	switch( dvw.num_channels() )
	{
		// Gray
		case 1:
		{
			char* pixelsY = (char*)boost::gil::interleaved_view_get_raw_data( dvw );
			frameBuffer.insert( "Y", Imf::Slice( pixType, pixelsY, bitsTypeSize, bitsTypeSize * src.width() ) );
			break;
		}
		// RGB
		// RGBA
		default:
			BOOST_THROW_EXCEPTION( exception::ImageFormat()
			    << exception::user( "ExrWriter: incompatible image type" ) );
			break;
	}
	file.setFrameBuffer( frameBuffer );
	// Finalize output
	file.writePixels( src.height() );
}


template<class View>
template<class WPixel>
void EXRWriterProcess<View>::writeImage( View& src, std::string& filepath, Imf::PixelType pixType )
{
	using namespace terry;

	std::size_t bitsTypeSize = 0;

	typedef image<WPixel, true> image_t;
	typedef typename image_t::view_t view_t;
	image_t img( src.width(), src.height() );
	view_t  dvw( view( img ) );
	copy_and_convert_pixels( src, dvw );
	Imf::Header header( src.width(), src.height() );
	switch( pixType )
	{
		case Imf::HALF:
			bitsTypeSize = sizeof( half );
			break;
		case Imf::FLOAT:
			bitsTypeSize = sizeof( float );
			break;
		case Imf::UINT:
			bitsTypeSize = sizeof( boost::uint32_t );
			break;
		default:
			break;
	}

	switch( dvw.num_channels() )
	{
		case 3:
		{
			header.channels().insert( "R", Imf::Channel( pixType ) );
			header.channels().insert( "G", Imf::Channel( pixType ) );
			header.channels().insert( "B", Imf::Channel( pixType ) );
			break;
		}
		case 4:
		{
			header.channels().insert( "R", Imf::Channel( pixType ) );
			header.channels().insert( "G", Imf::Channel( pixType ) );
			header.channels().insert( "B", Imf::Channel( pixType ) );
			header.channels().insert( "A", Imf::Channel( pixType ) );
			break;
		}
		// Gray
		case 1:
		default:
			BOOST_THROW_EXCEPTION( exception::ImageFormat()
			    << exception::user( "ExrWriter: incompatible image type" ) );
	}

	Imf::OutputFile file( filepath.c_str(), header );
	Imf::FrameBuffer frameBuffer;

	switch( dvw.num_channels() )
	{
		// RGB
		case 3:
		{
			char* pixelsR = (char*)boost::gil::planar_view_get_raw_data( dvw, 0 );
			char* pixelsG = (char*)boost::gil::planar_view_get_raw_data( dvw, 1 );
			char* pixelsB = (char*)boost::gil::planar_view_get_raw_data( dvw, 2 );
			frameBuffer.insert( "R", Imf::Slice( pixType, pixelsR, bitsTypeSize, bitsTypeSize * src.width() ) );
			frameBuffer.insert( "G", Imf::Slice( pixType, pixelsG, bitsTypeSize, bitsTypeSize * src.width() ) );
			frameBuffer.insert( "B", Imf::Slice( pixType, pixelsB, bitsTypeSize, bitsTypeSize * src.width() ) );
			break;
		}
		// RGBA
		case 4:
		{
			char* pixelsR = (char*)boost::gil::planar_view_get_raw_data( dvw, 0 );
			char* pixelsG = (char*)boost::gil::planar_view_get_raw_data( dvw, 1 );
			char* pixelsB = (char*)boost::gil::planar_view_get_raw_data( dvw, 2 );
			char* pixelsA = (char*)boost::gil::planar_view_get_raw_data( dvw, 3 );
			frameBuffer.insert( "R", Imf::Slice( pixType, pixelsR, bitsTypeSize, bitsTypeSize * src.width() ) );
			frameBuffer.insert( "G", Imf::Slice( pixType, pixelsG, bitsTypeSize, bitsTypeSize * src.width() ) );
			frameBuffer.insert( "B", Imf::Slice( pixType, pixelsB, bitsTypeSize, bitsTypeSize * src.width() ) );
			frameBuffer.insert( "A", Imf::Slice( pixType, pixelsA, bitsTypeSize, bitsTypeSize * src.width() ) );
			break;
		}
		// Gray
		case 1:
		default:
			BOOST_THROW_EXCEPTION( exception::ImageFormat()
			    << exception::user( "ExrWriter: incompatible image type" ) );
			break;
	}
	file.setFrameBuffer( frameBuffer );
	// Finalize output
	file.writePixels( src.height() );
}

}
}
}
}
