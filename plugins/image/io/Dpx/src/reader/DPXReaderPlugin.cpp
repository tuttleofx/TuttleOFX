#include "DPXReaderDefinitions.hpp"
#include "DPXReaderPlugin.hpp"
#include "DPXReaderProcess.hpp"
#include "tuttle/plugin/FilenameManager.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace tuttle::io;

namespace tuttle {
namespace plugin {
namespace dpx {
namespace reader {

using namespace boost::gil;
const static std::string kDpxReaderHelpString = "<b>DPX Reader</b> file reader.  <br />";

DPXReaderPlugin::DPXReaderPlugin( OfxImageEffectHandle handle )
: ImageEffect( handle )
, _dstClip( 0 )
{
	_dstClip  = fetchClip( kOfxImageEffectOutputClipName );
	_filepath = fetchStringParam( kInputFilename );
}

OFX::Clip* DPXReaderPlugin::getDstClip() const
{
	return _dstClip;
}

DPXReaderParams DPXReaderPlugin::getParams(const OfxTime time)
{
	DPXReaderParams params;
	params._filepath = _fPattern.getFilenameAt(time);
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void DPXReaderPlugin::render( const OFX::RenderArguments& args )
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
				DPXReaderProcess<rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				DPXReaderProcess<rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				DPXReaderProcess<rgba32f_view_t> fred( *this );
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

void DPXReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == "Help" )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kDpxReaderHelpString );
	}
	else if( paramName == kInputFilename )
	{
		_fPattern.reset(_filepath->getValue(), true);
		// Check if exist
		if( exists( _fPattern.getFilenameAt(args.time) ) )
		{
			// Read dims
			_dpxImg.readHeader( _fPattern.getFilenameAt(args.time) );
			_imageDims.x = _dpxImg.width();
			_imageDims.y = _dpxImg.height();
		}
	}
}

bool DPXReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	OfxRangeI rangei = _fPattern.getRange();
	range.min = (double)rangei.min;
	range.max = (double)rangei.max;
	return false;
}

void DPXReaderPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	const OfxRectD roi = { 0.0, 0.0, _imageDims.x * _dstClip->getPixelAspectRatio(), _imageDims.y };

	rois.setRegionOfInterest( *_dstClip, roi );
}

bool DPXReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	rod.x1 = 0;
	rod.x2 = _imageDims.x * _dstClip->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = _imageDims.y;
	return true;
}

void DPXReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	_fPattern.reset(_filepath->getValue(), true);
	// Check if exist
	if( exists( _fPattern.getCurrentFilename() ) )
	{
		_dpxImg.readHeader( _fPattern.getCurrentFilename() );
		OFX::BitDepthEnum bd = OFX::eBitDepthNone;
		switch(_dpxImg.componentsType())
		{
			case DpxImage::eCompTypeR8G8B8:
			case DpxImage::eCompTypeR8G8B8A8:
			case DpxImage::eCompTypeA8B8G8R8:
			{
				bd = OFX::eBitDepthUByte;
				break;
			}
			case DpxImage::eCompTypeR10G10B10:
			case DpxImage::eCompTypeR10G10B10A10:
			case DpxImage::eCompTypeA10B10G10R10:
			case DpxImage::eCompTypeR12G12B12:
			case DpxImage::eCompTypeR12G12B12A12:
			case DpxImage::eCompTypeA12B12G12R12:
			case DpxImage::eCompTypeR16G16B16:
			case DpxImage::eCompTypeR16G16B16A16:
			case DpxImage::eCompTypeA16B16G16R16:
			{
				bd = OFX::eBitDepthUShort;
				break;
			}
			default:
				bd = OFX::eBitDepthFloat;
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
