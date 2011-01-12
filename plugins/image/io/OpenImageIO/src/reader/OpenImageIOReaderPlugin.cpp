#include "OpenImageIOReaderPlugin.hpp"
#include "OpenImageIOReaderProcess.hpp"
#include "OpenImageIOReaderDefinitions.hpp"

#include <OpenImageIO/imageio.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace reader {

namespace bfs = boost::filesystem;
using namespace boost::gil;

OpenImageIOReaderPlugin::OpenImageIOReaderPlugin( OfxImageEffectHandle handle )
	: ReaderPlugin( handle )
{}

OpenImageIOReaderProcessParams OpenImageIOReaderPlugin::getProcessParams( const OfxTime time )
{
	OpenImageIOReaderProcessParams params;

	params._filepath = getAbsoluteFilenameAt( time );
	return params;
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
		ReaderPlugin::changedParam( args, paramName );
	}
}

bool OpenImageIOReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	using namespace OpenImageIO;
	const std::string filename( getAbsoluteFilenameAt( args.time ) );

	if( !bfs::exists( filename ) )
	{
		rod.x1 = 0;
		rod.x2 = 0;
		rod.y1 = 0;
		rod.y2 = 0;
		return true;
	}

	boost::scoped_ptr<ImageInput> in( ImageInput::create( filename ) );
	if( !in )
	{
		BOOST_THROW_EXCEPTION( exception::Value() );
	}
	ImageSpec spec;
	in->open( filename, spec );

	rod.x1 = 0;
	rod.x2 = spec.width * this->_clipDst->getPixelAspectRatio();
	rod.y1 = 0;
	rod.y2 = spec.height;

	in->close();
	return true;
}

void OpenImageIOReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	using namespace OpenImageIO;
	ReaderPlugin::getClipPreferences( clipPreferences );
	
	const std::string filename( getAbsoluteFirstFilename() );

	// spec.nchannels;
	//	switch( spec.format )
	//	{
	//		case TypeDesc::UINT8:
	//		case TypeDesc::UNKNOWN:
	//	}


	boost::scoped_ptr<ImageInput> in( ImageInput::create( filename ) );
	if( !in.get() )
	{
		BOOST_THROW_EXCEPTION( exception::Value() );
	}

	ImageSpec spec;
	in->open( filename, spec );

	switch( getExplicitConversion() )
	{
		case eParamReaderExplicitConversionAuto:
		{
			OFX::EBitDepth bd = OFX::eBitDepthNone;
			switch( spec.format.basetype )
			{
				//			case TypeDesc::UCHAR:
				case TypeDesc::UINT8:
				//			case TypeDesc::CHAR:
				case TypeDesc::INT8:
					bd = OFX::eBitDepthUByte;
					break;
				case TypeDesc::HALF:
				//			case TypeDesc::USHORT:
				case TypeDesc::UINT16:
				//			case TypeDesc::SHORT:
				case TypeDesc::INT16:
					bd = OFX::eBitDepthUShort;
					break;
				//			case TypeDesc::UINT:
				case TypeDesc::UINT32:
				//			case TypeDesc::INT:
				case TypeDesc::INT32:
				//			case TypeDesc::ULONGLONG:
				case TypeDesc::UINT64:
				//			case TypeDesc::LONGLONG:
				case TypeDesc::INT64:
				case TypeDesc::FLOAT:
				case TypeDesc::DOUBLE:
					bd = OFX::eBitDepthFloat;
					break;
				case TypeDesc::STRING:
				case TypeDesc::PTR:
				case TypeDesc::LASTBASE:
				case TypeDesc::UNKNOWN:
				case TypeDesc::NONE:
				default:
					BOOST_THROW_EXCEPTION( exception::ImageFormat() );
			}
			clipPreferences.setClipBitDepth( *this->_clipDst, bd );
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
	in->close();
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void OpenImageIOReaderPlugin::render( const OFX::RenderArguments& args )
{
	ReaderPlugin::render( args );

	doGilRender<OpenImageIOReaderProcess>( *this, args );
}

}
}
}
}
