#include "DPXReaderPlugin.hpp"
#include "DPXReaderProcess.hpp"
#include "DPXReaderDefinitions.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>
#include "dpxEngine/dpxImage.hpp"
#include "tuttle/plugin/context/ReaderPlugin.hpp"

using namespace boost::filesystem;
using namespace tuttle::io;

namespace tuttle {
namespace plugin {
namespace dpx {
namespace reader {

using namespace boost::gil;

DPXReaderPlugin::DPXReaderPlugin( OfxImageEffectHandle handle )
: ReaderPlugin( handle )
{
}

DPXReaderProcessParams DPXReaderPlugin::getProcessParams(const OfxTime time)
{
	DPXReaderProcessParams params;
	params._filepath = _filePattern.getFilenameAt(time);
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
		             kDPXReaderHelpString );
	}
	else
	{
		ReaderPlugin::changedParam(args, paramName);
	}
}

bool DPXReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	tuttle::io::DpxImage dpxImg;
	dpxImg.readHeader( _filePattern.getFirstFilename() );
	rod.x1 = 0;
	rod.x2 = dpxImg.width() * this->_dstClip->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = dpxImg.height();
	return true;
}

void DPXReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );
	// Check if exist
	if( bfs::exists( _filePattern.getFirstFilename() ) )
	{
		if ( _explicitConv->getValue() )
		{
			switch( _explicitConv->getValue() )
			{
				case 1:
				{
					clipPreferences.setClipBitDepth( *this->_dstClip, OFX::eBitDepthUByte );
					break;
				}
				case 2:
				{
					clipPreferences.setClipBitDepth( *this->_dstClip, OFX::eBitDepthUShort );
					break;
				}
				case 3:
				{
					clipPreferences.setClipBitDepth( *this->_dstClip, OFX::eBitDepthFloat );
					break;
				}
			}
		}
		else
		{
			DpxImage dpxImg;
			dpxImg.readHeader( _filePattern.getFirstFilename() );

			OFX::BitDepthEnum bd = OFX::eBitDepthNone;
			switch(dpxImg.componentsType())
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

			clipPreferences.setClipBitDepth( *_dstClip, bd );
		}
		clipPreferences.setClipComponents( *this->_dstClip, OFX::ePixelComponentRGBA );
		clipPreferences.setPixelAspectRatio( *this->_dstClip, 1.0 );
	}
}

}
}
}
}
