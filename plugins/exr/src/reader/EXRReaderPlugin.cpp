#include "EXRReaderPlugin.hpp"
#include "EXRReaderProcess.hpp"

#include <ImfInputFile.h>
#include <ImathBox.h>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {
using namespace Imf;
using namespace boost::gil;
const static std::string kExrReaderHelpString = "<b>EXR Reader</b> file reader.  <br />";

EXRReaderPlugin::EXRReaderPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle ),
	_dstClip( 0 )
{
	_dstClip  = fetchClip( "Output" );
	_filepath = fetchStringParam( "Input filename" );
	std::string sFilepath;
	_filepath->getValue( sFilepath );
	if( exists( sFilepath ) )
	{
		InputFile in (sFilepath.c_str());
		const Header &h = in.header();
		const Imath::V2i dataWindow = h.dataWindow().size();
		_imageDims.x = dataWindow.x;
		_imageDims.y = dataWindow.y;
	}
}

OFX::Clip* EXRReaderPlugin::getDstClip() const
{
	return _dstClip;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void EXRReaderPlugin::render( const OFX::RenderArguments& args )
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
				EXRReaderProcess<rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				EXRReaderProcess<rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				EXRReaderProcess<rgba32f_view_t> fred( *this );
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

void EXRReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == "Help" )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kExrReaderHelpString );
	}
	else if( paramName == "Input filename" )
	{
		std::string sFilepath;
		_filepath->getValue( sFilepath );
		// Check if exist
		if( exists( sFilepath ) )
		{
			// read dims
			InputFile in (sFilepath.c_str());
			const Header &h = in.header();
			const Imath::V2i dataWindow = h.dataWindow().size();
			_imageDims.x = dataWindow.x;
			_imageDims.y = dataWindow.y;
		}
	}
}

void EXRReaderPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	const OfxRectD roi = { 0.0, 0.0, _imageDims.x * _dstClip->getPixelAspectRatio(), _imageDims.y };

	rois.setRegionOfInterest( *_dstClip, roi );
}

bool EXRReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	rod.x1 = 0;
	rod.x2 = _imageDims.x * _dstClip->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = _imageDims.y;
	return true;
}

void EXRReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	clipPreferences.setClipComponents( *_dstClip, OFX::ePixelComponentRGBA );
	clipPreferences.setClipBitDepth( *_dstClip, OFX::eBitDepthUByte );
	clipPreferences.setPixelAspectRatio( *_dstClip, 720.0 / 720.0 );
}

}
}
}
}
