#include "OpenImageIOWriterDefinitions.hpp"
#include "OpenImageIOWriterPlugin.hpp"

#include <terry/globals.hpp>
#include <terry/clamp.hpp>
#include <terry/openexr/half.hpp>

#include <tuttle/plugin/exceptions.hpp>

#include <imageio.h>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/at.hpp>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace writer {


template<class View>
OpenImageIOWriterProcess<View>::OpenImageIOWriterProcess( OpenImageIOWriterPlugin& instance )
	: ImageGilFilterProcessor<View>( instance, eImageOrientationFromTopToBottom )
	, _plugin( instance )
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
	using namespace terry;
	params = _plugin.getProcessParams( this->_renderArgs.time );

	try
	{
		switch( (int) params._bitDepth )
		{
			case eTuttlePluginBitDepth8:
			{
				switch( params._components )
				{
					case eTuttlePluginComponentsGray:
						writeImage<gray8_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eTuttlePluginComponentsRGBA:
						writeImage<rgba8_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eTuttlePluginComponentsRGB:
						writeImage<rgb8_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					default:
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "ExrWriter: components not supported" ) );
				}
				break;
			}
			case eTuttlePluginBitDepth10:
			case eTuttlePluginBitDepth12:
			case eTuttlePluginBitDepth16:
			{
				switch( params._components )
				{
					case eTuttlePluginComponentsGray:
						writeImage<gray16_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eTuttlePluginComponentsRGBA:
						writeImage<rgba16_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eTuttlePluginComponentsRGB:
						writeImage<rgb16_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					default:
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "ExrWriter: components not supported" ) );
				}
				break;
			}
			case eTuttlePluginBitDepth16f:
			{
				switch( params._components )
				{
					case eTuttlePluginComponentsGray:
						writeImage<gray16h_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eTuttlePluginComponentsRGBA:
						writeImage<rgba16h_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eTuttlePluginComponentsRGB:
						writeImage<rgb16h_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					default:
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "ExrWriter: components not supported" ) );
				}
				break;
			}
			case eTuttlePluginBitDepth32:
			{
				switch( params._components )
				{
					case eTuttlePluginComponentsGray:
						writeImage<gray32_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eTuttlePluginComponentsRGBA:
						writeImage<rgba32_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eTuttlePluginComponentsRGB:
						writeImage<rgb32_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					default:
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "ExrWriter: components not supported" ) );
				}
				break;
			}
			case eTuttlePluginBitDepth32f:
			{
				switch( params._components )
				{
					case eTuttlePluginComponentsGray:
						writeImage<gray32f_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eTuttlePluginComponentsRGBA:
						writeImage<rgba32f_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eTuttlePluginComponentsRGB:
						writeImage<rgb32f_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					default:
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "ExrWriter: components not supported" ) );
				}
				break;
			}
			default:
				BOOST_THROW_EXCEPTION( exception::Unsupported()
					    << exception::user( "OIIO Writer: Unsupported bitdepth..." ) );
		}
	}
	catch( exception::Common& e )
	{
		e << exception::filename( params._filepath );
		throw;
	}
	catch(... )
	{
		BOOST_THROW_EXCEPTION( exception::Unknown()
			<< exception::user( "OIIO Writer: Unable to write image")
			<< exception::dev( boost::current_exception_diagnostic_information() )
			<< exception::filename(params._filepath) );
	}
	copy_pixels( this->_srcView, this->_dstView ); // @todo ?
}

/**
 *
 */
template<class View>
template<class WImage>
void OpenImageIOWriterProcess<View>::writeImage( View& src, const std::string& filepath, const ETuttlePluginBitDepth bitDepth, const ETuttlePluginComponents components )
{
	using namespace boost;
	using namespace OpenImageIO;

	boost::scoped_ptr<ImageOutput> out( ImageOutput::create( filepath ) );
	if( out.get() == NULL )
	{
		BOOST_THROW_EXCEPTION( exception::Bug()
							   << exception::user( "error from writer while opening file." ) );
	}
	WImage img( src.width(), src.height() );

	typename WImage::view_t vw( view( img ) );
	copy_and_convert_pixels( terry::clamp_view( src ), vw );

	OpenImageIO::TypeDesc oiioBitDepth;
	size_t sizeOfChannel = 0;
	int    bitsPerSample  = 0;
	switch( bitDepth )
	{
		case eTuttlePluginBitDepth8:
			oiioBitDepth = TypeDesc::UINT8;
			bitsPerSample = 8;
			sizeOfChannel = 1;
			break;
		case eTuttlePluginBitDepth10:
			oiioBitDepth = TypeDesc::UINT16;
			bitsPerSample = 10;
			sizeOfChannel = 2;
			break;
		case eTuttlePluginBitDepth12:
			oiioBitDepth = TypeDesc::UINT16;
			bitsPerSample = 12;
			sizeOfChannel = 2;
			break;
		case eTuttlePluginBitDepth16:
			oiioBitDepth = TypeDesc::UINT16;
			bitsPerSample = 16;
			sizeOfChannel = 2;
			break;
		case eTuttlePluginBitDepth16f:
			oiioBitDepth = TypeDesc::HALF;
			bitsPerSample = 16;
			sizeOfChannel = 2;
			break;
		case eTuttlePluginBitDepth32:
			oiioBitDepth = TypeDesc::UINT;
			bitsPerSample = 32;
			sizeOfChannel = 4;
			break;
		case eTuttlePluginBitDepth32f:
			oiioBitDepth = TypeDesc::FLOAT;
			bitsPerSample = 32;
			sizeOfChannel = 4;
			break;
		default:
			BOOST_THROW_EXCEPTION( exception::Bug()
				<< exception::user() + "Incorrect bit depth param value." );
	}

	ImageSpec spec( src.width(), src.height(), gil::num_channels<WImage>::value, oiioBitDepth );

	spec.attribute( "oiio:BitsPerSample", bitsPerSample );
	spec.attribute( "oiio:UnassociatedAlpha", params._premultiply );
	spec.attribute( "CompressionQuality", params._quality );
	spec.attribute( "Orientation", params._orientation );
	
	if( ! out->open( filepath, spec ) )
	{
		BOOST_THROW_EXCEPTION( exception::Unknown()
			<< exception::user( "OIIO Writer: " + out->geterror () )
			<< exception::filename(params._filepath) );
	}

	const stride_t xstride = gil::is_planar<WImage>::value ? sizeOfChannel : vw.num_channels() * sizeOfChannel;
	const stride_t ystride = vw.pixels().row_size();
	const stride_t zstride = ystride * vw.height();

	typedef typename boost::gil::channel_type<WImage>::type channel_t;

	out->write_image(
			oiioBitDepth,
			&( ( *vw.begin() )[0] ),// get the adress of the first channel value from the first pixel
			xstride,
			ystride,
			zstride,
			&progressCallback,
			this
		);

	out->close();
}



}
}
}
}
