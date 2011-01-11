#include "ImageMagickReaderPlugin.hpp"
#include "ImageMagickReaderProcess.hpp"
#include "ImageMagickReaderDefinitions.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

#include <cstring>

namespace tuttle {
namespace plugin {
namespace imagemagick {
namespace reader {

namespace bfs = boost::filesystem;
using namespace boost::gil;

ImageMagickReaderPlugin::ImageMagickReaderPlugin( OfxImageEffectHandle handle )
	: ReaderPlugin( handle )
{
	InitializeMagick( "" );
}

ImageMagickReaderProcessParams ImageMagickReaderPlugin::getProcessParams( const OfxTime time )
{
	ImageMagickReaderProcessParams params;

	params._filepath = getAbsoluteFilenameAt( time );
	params._flip = _paramFlip->getValue();

	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ImageMagickReaderPlugin::render( const OFX::RenderArguments& args )
{
	ReaderPlugin::render( args );
	
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth bitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent components = _clipDst->getPixelComponents();

	switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender<ImageMagickReaderProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		case OFX::ePixelComponentAlpha:
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

void ImageMagickReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kImageMagickReaderHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kImageMagickReaderHelpString );
	}
	else
	{
		ReaderPlugin::changedParam( args, paramName );
	}
}

bool ImageMagickReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	ImageInfo* imageInfo = AcquireImageInfo();

	GetImageInfo( imageInfo );
	std::string filename = getAbsoluteFilenameAt( args.time );
	strcpy( imageInfo->filename, filename.c_str() );
	ExceptionInfo* exceptionsInfo = AcquireExceptionInfo();
	GetExceptionInfo( exceptionsInfo );

	Image* image = PingImage( imageInfo, exceptionsInfo );

	if( !image )
	{
		rod.x1 = 0;
		rod.x2 = 0;
		rod.y1 = 0;
		rod.y2 = 0;
		return true;
	}

	point2<ptrdiff_t> imagemagickDims( image->columns, image->rows );
	rod.x1 = 0;
	rod.x2 = imagemagickDims.x * this->_clipDst->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = imagemagickDims.y;

	image          = DestroyImage( image );
	imageInfo      = DestroyImageInfo( imageInfo );
	exceptionsInfo = DestroyExceptionInfo( exceptionsInfo );

	return true;
}

void ImageMagickReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );
	const std::string filename = getAbsoluteFirstFilename();

	ImageInfo* imageInfo = AcquireImageInfo();
	GetImageInfo( imageInfo );
	strcpy( imageInfo->filename, filename.c_str() );
	ExceptionInfo* exceptionsInfo = AcquireExceptionInfo();
	GetExceptionInfo( exceptionsInfo );

	Image* image = PingImage( imageInfo, exceptionsInfo );

	switch( getExplicitConversion() )
	{
		case eParamReaderExplicitConversionAuto:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthFloat ); // by default
			if( image )
			{
				unsigned long bitDepth = GetImageDepth( image, exceptionsInfo ); // if image information use it
				if( bitDepth <= 8 )
				{
					clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUByte );
				}
				else if( bitDepth <= 16 )
				{
					clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUShort );
				}
			}
			break;
		}
		case eParamReaderExplicitConversionByte:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUByte );
			break;
		}
		case eParamReaderExplicitConversionShort:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUShort );
			break;
		}
		case eParamReaderExplicitConversionFloat:
		{
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthFloat );
			break;
		}
	}
	clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA ); /// @todo tuttle: retrieve info, gray / RGB / RGBA...
	clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 ); /// @todo tuttle: retrieve info

	if( image )
		image = DestroyImage( image );
	imageInfo      = DestroyImageInfo( imageInfo );
	exceptionsInfo = DestroyExceptionInfo( exceptionsInfo );
}

}
}
}
}
