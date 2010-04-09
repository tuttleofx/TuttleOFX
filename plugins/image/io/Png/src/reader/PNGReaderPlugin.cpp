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
#include "PngEngine/png_adds.hpp"

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
}

OFX::Clip* PNGReaderPlugin::getDstClip() const
{
	return _dstClip;
}

PNGReaderParams PNGReaderPlugin::getParams(const OfxTime time)
{
	PNGReaderParams params;
	params._filepath = _fPattern.getFilenameAt(time);
	return params;
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
		_fPattern.reset(_filepath->getValue(), true);
		// Check if exist
		if( exists( _fPattern.getFilenameAt(args.time) ) )
		{
			_pngDims = png_read_dimensions( _fPattern.getFilenameAt(args.time) );
		}
	}
}

bool PNGReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	OfxRangeI rangei = _fPattern.getRange();
	range.min = (double)rangei.min;
	range.max = (double)rangei.max;
	return false;
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
	_fPattern.reset(_filepath->getValue(), true);
	// Check if exist
	if( exists( _fPattern.getCurrentFilename() ) )
	{
		_pngDims = png_read_dimensions( _fPattern.getCurrentFilename() );
		OFX::BitDepthEnum bd = OFX::eBitDepthNone;
		switch( png_read_precision( _fPattern.getCurrentFilename() ))
		{
			case 8:
				bd = OFX::eBitDepthUByte;
				break;
			case 16:
				bd = OFX::eBitDepthUShort;
				break;
			default:
				bd = OFX::eBitDepthFloat;
				break;
		}
		clipPreferences.setClipComponents( *_dstClip, OFX::ePixelComponentRGBA );
		clipPreferences.setClipBitDepth( *_dstClip, bd );
		clipPreferences.setPixelAspectRatio( *_dstClip, 1.0 );
	}
}

}
}
}
}
