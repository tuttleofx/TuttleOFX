#include "DPXReaderPlugin.hpp"
#include "DPXReaderProcess.hpp"
#include "DPXReaderDefinitions.hpp"

#include "dpxEngine/dpxImage.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace reader {

using namespace boost::filesystem;
using namespace tuttle::io;
using namespace boost::gil;

DPXReaderPlugin::DPXReaderPlugin( OfxImageEffectHandle handle )
	: ReaderPlugin( handle )
{}

DPXReaderProcessParams DPXReaderPlugin::getProcessParams( const OfxTime time )
{
	DPXReaderProcessParams params;

	params._filepath = getAbsoluteFilenameAt( time );
	params._flip = _paramFlip->getValue();

	return params;
}

void DPXReaderPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	ReaderPlugin::changedParam( args, paramName );
	if( paramName == kParamDisplayHeader )
	{
		tuttle::io::DpxImage dpxImg;
		dpxImg.readHeader( getAbsoluteFilenameAt( args.time ) );
		std::ostringstream headerStr;
		headerStr << "DPX HEADER:" << std::endl;
		headerStr << dpxImg.getHeader();

		TUTTLE_COUT( headerStr.str() );

		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             headerStr.str() );
	}
}

bool DPXReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	tuttle::io::DpxImage dpxImg;

	dpxImg.readHeader( getAbsoluteFilenameAt( args.time ) );
	rod.x1 = 0;
	rod.x2 = dpxImg.width() * this->_clipDst->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = dpxImg.height();
	return true;
}

void DPXReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );
	const std::string filename( getAbsoluteFirstFilename() );

	switch( getExplicitConversion() )
	{
		case eParamReaderExplicitConversionAuto:
		{
			DpxImage dpxImg;
			dpxImg.readHeader( filename );

			OFX::EBitDepth bd = OFX::eBitDepthNone;
			switch( dpxImg.componentsType() )
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

			clipPreferences.setClipBitDepth( *_clipDst, bd );
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
	clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
	clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 );
}
/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void DPXReaderPlugin::render( const OFX::RenderArguments& args )
{
	ReaderPlugin::render( args );

	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth bitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent components = _clipDst->getPixelComponents();

	switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender<DPXReaderProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
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

}
}
}
}
