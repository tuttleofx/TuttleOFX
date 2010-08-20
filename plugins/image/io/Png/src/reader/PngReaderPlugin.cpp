#include "PngReaderPlugin.hpp"
#include "PngReaderProcess.hpp"
#include "PngReaderDefinitions.hpp"

#include "PngEngine/png_adds.hpp"

#include <tuttle/plugin/context/ReaderPlugin.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>
#include <boost/exception/all.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace reader {

namespace bfs = boost::filesystem;
using namespace boost::gil;

PngReaderPlugin::PngReaderPlugin( OfxImageEffectHandle handle )
: ReaderPlugin( handle )
{
}

PngReaderProcessParams PngReaderPlugin::getProcessParams(const OfxTime time)
{
	PngReaderProcessParams params;
	params._filepath = getAbsoluteFilenameAt(time);
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void PngReaderPlugin::render( const OFX::RenderArguments& args )
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
				PngReaderProcess<rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				PngReaderProcess<rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				PngReaderProcess<rgba32f_view_t> fred( *this );
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
				PngReaderProcess<gray8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				PngReaderProcess<gray16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				PngReaderProcess<gray32f_view_t> fred( *this );
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

void PngReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kPngReaderHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kPngReaderHelpString );
	}
	else
	{
		ReaderPlugin::changedParam(args, paramName);
	}
}

bool PngReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	const std::string filename( getAbsoluteFilenameAt(args.time) );
	
	try
	{
		point2<ptrdiff_t> pngDims = png_read_dimensions( filename );
		rod.x1 = 0;
		rod.x2 = pngDims.x * this->_clipDst->getPixelAspectRatio();
		rod.y1 = 0;
		rod.y2 = pngDims.y;
		COUT_VAR( rod );
	}
	catch( boost::exception& e )
	{
		e << exception::filename(filename);
		throw;
	}
	return true;
}

void PngReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );
	const std::string filename( getAbsoluteFirstFilename() );
	
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
			OFX::BitDepthEnum bd = OFX::eBitDepthNone;
			int bitDepth = png_read_precision( filename );
			switch( bitDepth )
			{
				case 8:
					bd = OFX::eBitDepthUByte;
					break;
				case 16:
					bd = OFX::eBitDepthUShort;
					break;
				default:
					BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrImageFormat ) );
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
