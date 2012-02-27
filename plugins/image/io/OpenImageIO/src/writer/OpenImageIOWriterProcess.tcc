#include "OpenImageIOWriterDefinitions.hpp"
#include "OpenImageIOWriterPlugin.hpp"

#include <terry/globals.hpp>
#include <terry/clamp.hpp>
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
	OpenImageIOWriterProcessParams params = _plugin.getProcessParams( this->_renderArgs.time );

	try
	{
		/// @todo tuttle: use params._components
		switch( (int) params._bitDepth )
		{
			case eParamBitDepth8:
			{
				switch( params._components )
				{
					case eParamComponentsGray:
						writeImage<gray8_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eParamComponentsRGBA:
						writeImage<rgba8_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eParamComponentsRGB:
						writeImage<rgb8_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
				}
				break;
			}
			case eParamBitDepth16:
			{
				switch( params._components )
				{
					case eParamComponentsGray:
						writeImage<gray16_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eParamComponentsRGBA:
						writeImage<rgba16_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eParamComponentsRGB:
						writeImage<rgb16_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
				}
				break;
			}
			case eParamBitDepth32:
			{
				switch( params._components )
				{
					case eParamComponentsGray:
						writeImage<gray32_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eParamComponentsRGBA:
						writeImage<rgba32_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
					case eParamComponentsRGB:
						writeImage<rgb32_image_t>( this->_srcView, params._filepath, params._bitDepth, params._components );
						break;
				}
				break;
			}
			default:
				BOOST_THROW_EXCEPTION( exception::Unsupported()
					    << exception::user( "DPX Writer: Unsupported bitdepth..." ) );
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
			<< exception::user( "Unable to write image")
			<< exception::dev( boost::current_exception_diagnostic_information() )
			<< exception::filename(params._filepath) );
	}
	copy_pixels( this->_srcView, this->_dstView );
}

/**
 *
 */
template<class View>
template<class WImage>
void OpenImageIOWriterProcess<View>::writeImage( View& src, const std::string& filepath, const EParamBitDepth bitDepth, const EParamComponents components )
{
	using namespace boost;
	using namespace OpenImageIO;
	boost::scoped_ptr<ImageOutput> out( ImageOutput::create( filepath ) );
	if( out.get() == NULL )
	{
		BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrValue ) );
	}

	WImage img( src.width(), src.height() );

	typename WImage::view_t vw( view( img ) );
	copy_and_convert_pixels( terry::clamp_view( src ), vw );

	OpenImageIO::TypeDesc oiioBitDepth;
	size_t sizeOfChannel = 0;
	switch( bitDepth )
	{
		case eParamBitDepth8:
			oiioBitDepth = TypeDesc::UINT8;
			sizeOfChannel = 1;
			break;
		case eParamBitDepth16:
			oiioBitDepth = TypeDesc::UINT16;
			sizeOfChannel = 2;
			break;
		case eParamBitDepth32:
			oiioBitDepth = TypeDesc::FLOAT;
			sizeOfChannel = 4;
			break;
		default:
			BOOST_THROW_EXCEPTION( exception::Bug()
				<< exception::user() + "Incorrect bit depth param value." );
	}

	ImageSpec spec( src.width(), src.height(), gil::num_channels<WImage>::value, oiioBitDepth );
	out->open( filepath, spec );

	const stride_t xstride = gil::is_planar<WImage>::value ? sizeOfChannel : vw.num_channels() * sizeOfChannel;
	const stride_t ystride = vw.pixels().row_size();
	const stride_t zstride = ystride * vw.height();

	typedef typename boost::gil::channel_type<WImage>::type channel_t;

	TUTTLE_COUT( sizeOfChannel << "  " << vw.num_channels() << "  " << gil::is_planar<WImage>::value );

	TUTTLE_COUT( xstride << " " << ystride << " " << zstride );

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
