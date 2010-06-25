#include "OpenImageIOReaderPlugin.hpp"
#include "OpenImageIOReaderProcess.hpp"
#include "OpenImageIOReaderDefinitions.hpp"

#include <tuttle/plugin/context/ReaderPlugin.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace reader {

namespace bfs = boost::filesystem;
using namespace boost::gil;

OpenImageIOReaderPlugin::OpenImageIOReaderPlugin( OfxImageEffectHandle handle )
: ReaderPlugin( handle )
{
}

OpenImageIOReaderProcessParams OpenImageIOReaderPlugin::getProcessParams(const OfxTime time)
{
	OpenImageIOReaderProcessParams params;
	params._filepath = _filePattern.getFilenameAt(time);
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void OpenImageIOReaderPlugin::render( const OFX::RenderArguments& args )
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
				OpenImageIOReaderProcess<rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				OpenImageIOReaderProcess<rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				OpenImageIOReaderProcess<rgba32f_view_t> fred( *this );
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
				OpenImageIOReaderProcess<gray8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				OpenImageIOReaderProcess<gray16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				OpenImageIOReaderProcess<gray32f_view_t> fred( *this );
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

void OpenImageIOReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kOpenImageIOReaderHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kOpenImageIOReaderHelpString );
	}
	else
	{
		ReaderPlugin::changedParam(args, paramName);
	}
}

bool OpenImageIOReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	point2<ptrdiff_t> dims; //= openImageIO_read_dimensions( _filePattern.getFilenameAt(args.time) );
	rod.x1 = 0;
	rod.x2 = dims.x * this->_clipDst->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = dims.y;
	return true;
}

void OpenImageIOReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );
	// Check if exist
	if( bfs::exists( _filePattern.getFirstFilename() ) )
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
			OFX::BitDepthEnum bd = OFX::eBitDepthNone;
			int bitDepth = 8; //openImageIO_read_precision( _filePattern.getFirstFilename() );
			switch( bitDepth )
			{
				case 8:
					bd = OFX::eBitDepthUByte;
					break;
				case 16:
					bd = OFX::eBitDepthUShort;
					break;
				default:
					throw OFX::Exception::Suite( kOfxStatErrImageFormat );
					break;
			}
			clipPreferences.setClipBitDepth( *this->_clipDst, bd );
		}
		clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
		clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 );
	}
}

}
}
}
}
