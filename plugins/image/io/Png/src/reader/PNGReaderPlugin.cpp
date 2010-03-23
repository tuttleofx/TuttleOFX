/**
 * @file PNGReaderPlugin.cpp
 * @brief PNG reader ofx plugin
 * @author Eloi Du Bois
 * @date    27/09/09 10:42
 *
 */
#include "PNGReaderPlugin.hpp"
#include "PNGReaderProcess.hpp"
#include "PNGReaderDefinitions.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace tuttle {
namespace plugin {
namespace png {
namespace reader {

using namespace boost::gil;
const static std::string kPngReaderHelpString = "<b>PNG Reader</b> file reader.  <br />";

PNGReaderPlugin::PNGReaderPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle ),
	_dstClip( 0 )
{
	_dstClip  = fetchClip( kOfxImageEffectOutputClipName );
	_filepath = fetchStringParam( kInputFilename );
	std::string sFilepath;
	_filepath->getValue( sFilepath );
	if( exists( sFilepath ) )
	{
		_pngDims = png_read_dimensions( sFilepath );
	}
}

OFX::Clip* PNGReaderPlugin::getDstClip() const
{
	return _dstClip;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void PNGReaderPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::BitDepthEnum dstBitDepth         = _dstClip->getPixelDepth();
	OFX::PixelComponentEnum dstComponents = _dstClip->getPixelComponents();

	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				PNGReaderProcess<rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				PNGReaderProcess<rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				PNGReaderProcess<rgba32f_view_t> fred( *this );
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
				PNGReaderProcess<gray8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				PNGReaderProcess<gray16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				PNGReaderProcess<gray32f_view_t> fred( *this );
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
}

void PNGReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == "Help" )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kPngReaderHelpString );
	}
	else if( paramName == kInputFilename )
	{
		std::string sFilepath;
		_filepath->getValue( sFilepath );
		// Check if exist
		if( exists( sFilepath ) )
		{
			_pngDims = png_read_dimensions( sFilepath );
		}
	}
}

bool PNGReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	rod.x1 = 0;
	rod.x2 = _pngDims.x * _dstClip->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = _pngDims.y;
	return true;
}

void PNGReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	clipPreferences.setClipComponents( *_dstClip, OFX::ePixelComponentRGBA );
	clipPreferences.setClipBitDepth( *_dstClip, OFX::eBitDepthUByte );
	clipPreferences.setPixelAspectRatio( *_dstClip, 720.0 / 720.0 );
}

}
}
}
}
