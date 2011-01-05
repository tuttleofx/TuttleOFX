#include "EXRWriterDefinitions.hpp"
#include "EXRWriterProcess.hpp"
#include "EXRWriterPlugin.hpp"
#include "../half/gilHalf.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/clamp.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/cstdint.hpp>
#include <boost/assert.hpp>

#include <cstdlib>
#include <cmath>
#include <vector>
#include <iostream>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

using namespace boost::gil;

template<class View>
EXRWriterProcess<View>::EXRWriterProcess( EXRWriterPlugin& instance )
	: ImageGilFilterProcessor<View>( instance )
	, _plugin( instance )
{
	this->setNoMultiThreading();
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
	EXRWriterProcessParams params = _plugin.getProcessParams( this->_renderArgs.time );
	
	try
	{
		switch( params._bitDepth )
		{
			case eParamBitDepth16f:
			{
				switch( params._componentsType )
				{
					case eGray:
					{
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "ExrWriter: Gray not supported!" ) );
						// writeImage<gray16h_pixel_t>(this->_srcView, filepath, Imf::HALF);
						break;
					}
					case eRGB:
					{
						writeImage<rgb16h_pixel_t>( this->_srcView, params._filepath, Imf::HALF );
						break;
					}
					case eRGBA:
					{
						writeImage<rgba16h_pixel_t>( this->_srcView, params._filepath, Imf::HALF );
						break;
					}
				}
				break;
			}
			case eParamBitDepth32f:
			{
				switch( params._componentsType )
				{
					case eGray:
					{
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "ExrWriter: Gray not supported!" ) );
						// writeImage<gray32f_pixel_t>(this->_srcView, filepath, Imf::FLOAT);
						break;
					}
					case eRGB:
					{
						writeImage<rgb32f_pixel_t>( this->_srcView, params._filepath, Imf::FLOAT );
						break;
					}
					case eRGBA:
					{
						writeImage<rgba32f_pixel_t>( this->_srcView, params._filepath, Imf::FLOAT );
						break;
					}
				}
				break;
			}
			case eParamBitDepth32:
			{
				switch( params._componentsType )
				{
					case eGray:
					{
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "ExrWriter: Gray not supported!" ) );
						// writeImage<gray32_pixel_t>(this->_srcView, filepath, Imf::FLOAT);
						break;
					}
					case eRGB:
					{
						writeImage<rgb32_pixel_t>( this->_srcView, params._filepath, Imf::UINT );
						break;
					}
					case eRGBA:
					{
						writeImage<rgba32_pixel_t>( this->_srcView, params._filepath, Imf::UINT );
						break;
					}
				}
				break;
			}
		}
	}
	catch( exception::Common& e )
	{
		e << exception::filename( params._filepath );
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
	// @todo: This is sometimes not neccessary... Checkbox it.
	copy_and_convert_pixels( this->_srcView, this->_dstView );
}

template<class View>
template<class WPixel>
void EXRWriterProcess<View>::writeImage( View& src, std::string& filepath, Imf::PixelType pixType )
{
	size_t bitsTypeSize = 0;

	typedef image<WPixel, true> image_t;
	typedef typename image_t::view_t view_t;
	image_t img( src.width(), src.height() );
	view_t dvw( view( img ) );
	View flippedView = flipped_up_down_view( src );
	copy_and_convert_pixels( clamp_view( flippedView ), dvw );
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
		default:
			return;
	}

	Imf::OutputFile file( filepath.c_str(), header );
	Imf::FrameBuffer frameBuffer;

	switch( dvw.num_channels() )
	{
		// Gray
		case 1:
		{
			char* pixelsY = (char*)boost::gil::planar_view_get_raw_data( dvw, 0 );
			frameBuffer.insert( "Y", Imf::Slice( pixType, pixelsY, bitsTypeSize, bitsTypeSize * src.width() ) );
			break;
		}
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
