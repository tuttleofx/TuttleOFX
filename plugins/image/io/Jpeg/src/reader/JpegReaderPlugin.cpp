#include "JpegReaderPlugin.hpp"
#include "JpegReaderProcess.hpp"
#include "JpegReaderDefinitions.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>
#include "tuttle/plugin/context/ReaderPlugin.hpp"

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace reader {

namespace bfs = boost::filesystem;
using namespace boost::gil;

JpegReaderPlugin::JpegReaderPlugin( OfxImageEffectHandle handle )
: ReaderPlugin( handle )
{
}

JpegReaderProcessParams JpegReaderPlugin::getProcessParams(const OfxTime time)
{
	JpegReaderProcessParams params;
	params._filepath = getFilenameAt(time);
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void JpegReaderPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::BitDepthEnum dstBitDepth         = this->_clipDst->getPixelDepth();
	OFX::PixelComponentEnum dstComponents = this->_clipDst->getPixelComponents();
	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				JpegReaderProcess<rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				JpegReaderProcess<rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				JpegReaderProcess<rgba32f_view_t> fred( *this );
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
	}
	else if( dstComponents == OFX::ePixelComponentAlpha )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				JpegReaderProcess<gray8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				JpegReaderProcess<gray16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				JpegReaderProcess<gray32f_view_t> fred( *this );
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
	}
	else
	{
		COUT_FATALERROR( dstComponents << " not recognize." );
	}
}

void JpegReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kJpegReaderHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kJpegReaderHelpString );
	}
	else
	{
		ReaderPlugin::changedParam(args, paramName);
	}
}

bool JpegReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	point2<ptrdiff_t> jpegDims = jpeg_read_dimensions( getFilenameAt(args.time) );
	rod.x1 = 0;
	rod.x2 = jpegDims.x * this->_clipDst->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = jpegDims.y;
	return true;
}

void JpegReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );
	const std::string filename( getFilePattern().getFirstFilename() );
	
	// Check if exist
	if( bfs::exists( filename ) )
	{
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
			clipPreferences.setClipBitDepth( *this->_clipDst, OFX::eBitDepthUByte );
		}
		clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA ); /// RGB
		clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 );
	}
}

}
}
}
}
