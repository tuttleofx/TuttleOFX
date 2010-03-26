#include "EXRWriterDefinitions.hpp"
#include "EXRWriterProcess.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include "../half/gilHalf.hpp"
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/cstdint.hpp>

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
	_filepath       = instance.fetchStringParam( kOutputFilename );
	_bitDepth       = instance.fetchChoiceParam( kParamBitDepth );
	_componentsType = instance.fetchChoiceParam( kParamComponentsType );
}

/**
 * @brief Function called by rendering thread each time
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void EXRWriterProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	try
	{
		View src = subimage_view( this->_srcView, procWindow.x1, procWindow.y1,
		                          procWindow.x2 - procWindow.x1,
		                          procWindow.y2 - procWindow.y1 );
		View dst = subimage_view( this->_dstView, procWindow.x1, procWindow.y1,
		                          procWindow.x2 - procWindow.x1,
		                          procWindow.y2 - procWindow.y1 );
		std::string filepath;
		this->_filepath->getValue( filepath );

		switch( (EBitDepth)_bitDepth->getValue() )
		{
			case eHalfFloat:
			{
				switch( (ECompType)_componentsType->getValue() )
				{
					case eGray:
					{
						//						writeImage<gray16h_pixel_t>(this->_srcView, filepath, Imf::HALF);
						break;
					}
					case eRGB:
					{
						writeImage<rgb16h_pixel_t>( this->_srcView, filepath, Imf::HALF );
						break;
					}
					case eRGBA:
					{
						writeImage<rgba16h_pixel_t>( this->_srcView, filepath, Imf::HALF );
						break;
					}
				}
				break;
			}
			case eFloat:
			{
				switch( (ECompType)_componentsType->getValue() )
				{
					case eGray:
					{
						//						writeImage<gray32f_pixel_t>(this->_srcView, filepath, Imf::FLOAT);
						break;
					}
					case eRGB:
					{
						writeImage<rgb32f_pixel_t>( this->_srcView, filepath, Imf::FLOAT );
						break;
					}
					case eRGBA:
					{
						writeImage<rgba32f_pixel_t>( this->_srcView, filepath, Imf::FLOAT );
						break;
					}
				}
				break;
			}
			case eInt32:
			{
				switch( (ECompType)_componentsType->getValue() )
				{
					case eGray:
					{
						//						writeImage<gray32_pixel_t>(this->_srcView, filepath, Imf::FLOAT);
						break;
					}
					case eRGB:
					{
						writeImage<rgb32f_pixel_t>( this->_srcView, filepath, Imf::UINT );
						break;
					}
					case eRGBA:
					{
						writeImage<rgba32_pixel_t>( this->_srcView, filepath, Imf::UINT );
						break;
					}
				}
				break;
			}
		}
		// @todo: This is sometimes not neccessary... Checkbox it.
		copy_and_convert_pixels( src, dst );
	}
	catch( PluginException& err )
	{
		COUT_EXCEPTION( err );
	}
}

template<class View>
template<class Pixel>
void EXRWriterProcess<View>::writeImage( View& src, std::string& filepath, Imf::PixelType pixType ) throw( tuttle::plugin::PluginException )
{
	size_t bitsTypeSize = 0;

	typedef image<Pixel, true> image_t;
	typedef typename image_t::view_t view_t;
	image_t img( src.width(), src.height() );
	view_t dvw( view( img ) );
	View flippedView = flipped_up_down_view( src );
	copy_and_convert_pixels( flippedView, dvw );
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
			throw( PluginException( "ExrWriter: incompatible image type" ) );
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
