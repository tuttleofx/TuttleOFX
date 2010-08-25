#include "OpenImageIOReaderPlugin.hpp"
#include "OpenImageIOReaderProcess.hpp"
#include "OpenImageIOReaderDefinitions.hpp"

#include <tuttle/plugin/context/ReaderPlugin.hpp>

#include <OpenImageIO/imageio.h>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

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
{
}

OpenImageIOReaderProcessParams OpenImageIOReaderPlugin::getProcessParams(const OfxTime time)
{
	OpenImageIOReaderProcessParams params;
	params._filepath = getAbsoluteFilenameAt(time);
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void OpenImageIOReaderPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth dstBitDepth         = this->_clipDst->getPixelDepth();
	OFX::EPixelComponent dstComponents = this->_clipDst->getPixelComponents();
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
	using namespace OpenImageIO;
	const std::string filename( getAbsoluteFilenameAt(args.time) );

	if( ! bfs::exists( filename ) )
	{
		rod.x1 = 0;
		rod.x2 = 0;
		rod.y1 = 0;
		rod.y2 = 0;
		return true;
	}

	boost::scoped_ptr<ImageInput> in( ImageInput::create( filename ) );
	if( !in )
		BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrValue ) );
	ImageSpec spec;
	in->open( filename, spec );

	point2<ptrdiff_t> dims;
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
	const std::string filename( getAbsoluteFirstFilename() );

	// spec.nchannels;
//	switch( spec.format )
//	{
//		case TypeDesc::UINT8:
//		case TypeDesc::UNKNOWN:
//	}

	ReaderPlugin::getClipPreferences( clipPreferences );
	// Check if exist
	if( ! bfs::exists( filename ) )
		BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrValue ) );
	boost::scoped_ptr<ImageInput> in( ImageInput::create( filename ) );
	if( !in )
		BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrValue ) );
	ImageSpec spec;
	in->open( filename, spec );
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
				BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrImageFormat ) );
		}
		clipPreferences.setClipBitDepth( *this->_clipDst, bd );
	}
	clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
	clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 );
	in->close();
}

}
}
}
}
