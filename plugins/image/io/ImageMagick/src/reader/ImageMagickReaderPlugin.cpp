#include "ImageMagickReaderPlugin.hpp"
#include "ImageMagickReaderProcess.hpp"
#include "ImageMagickReaderDefinitions.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>
#include "tuttle/plugin/context/ReaderPlugin.hpp"

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

ImageMagickReaderProcessParams ImageMagickReaderPlugin::getProcessParams(const OfxTime time)
{
	ImageMagickReaderProcessParams params;
	params._filepath = getAbsoluteFilenameAt(time);
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ImageMagickReaderPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth dstBitDepth         = this->_clipDst->getPixelDepth();
	OFX::EPixelComponent dstComponents = this->_clipDst->getPixelComponents();
	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				ImageMagickReaderProcess<rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				ImageMagickReaderProcess<rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				ImageMagickReaderProcess<rgba32f_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}/*
	else if( dstComponents == OFX::ePixelComponentAlpha )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				ImageMagickReaderProcess<gray8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				ImageMagickReaderProcess<gray16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				ImageMagickReaderProcess<gray32f_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}*/
	else
	{
		COUT_FATALERROR( dstComponents << " not recognize." );
	}
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
		ReaderPlugin::changedParam(args, paramName);
	}
}

bool ImageMagickReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	ImageInfo* imageInfo = AcquireImageInfo();
	GetImageInfo( imageInfo );
	std::string filename = getAbsoluteFilenameAt(args.time);
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
	
	image = DestroyImage( image );
	imageInfo = DestroyImageInfo( imageInfo );
	exceptionsInfo = DestroyExceptionInfo( exceptionsInfo );

	return true;
}

void ImageMagickReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );
	const std::string filename = getAbsoluteFirstFilename();

	// Check if exist
	if( bfs::exists( filename ) )
	{
		ImageInfo* imageInfo = AcquireImageInfo();
		GetImageInfo( imageInfo );
		strcpy( imageInfo->filename, filename.c_str() );
		ExceptionInfo* exceptionsInfo = AcquireExceptionInfo();
		GetExceptionInfo( exceptionsInfo );
		
		Image* image = PingImage( imageInfo, exceptionsInfo );

		if ( _paramExplicitConv->getValue() )
		{
			switch( _paramExplicitConv->getValue() )
			{
				case 1:
				{
					clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUByte );
					break;
				}
				case 2:
				{
					clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUShort );
					break;
				}
				case 3:
				{
					clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthFloat );
					break;
				}
			}
		}
		else
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
		}
		clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA ); /// RGB
		clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 );

		if( image )
			image = DestroyImage( image );
		imageInfo = DestroyImageInfo( imageInfo );
		exceptionsInfo = DestroyExceptionInfo( exceptionsInfo );
	}
}

}
}
}
}
