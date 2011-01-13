#include "Jpeg2000ReaderPlugin.hpp"
#include "Jpeg2000ReaderProcess.hpp"
#include "Jpeg2000ReaderDefinitions.hpp"

#include <tuttle/plugin/context/WriterDefinition.hpp>
#include <tuttle/common/utils/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg2000 {
namespace reader {

using namespace tuttle::io;
using namespace boost::gil;
using namespace boost::filesystem;

Jpeg2000ReaderPlugin::Jpeg2000ReaderPlugin( OfxImageEffectHandle handle )
: ReaderPlugin( handle )
{
}

Jpeg2000ReaderPlugin::~Jpeg2000ReaderPlugin()
{
	_reader.close();
}

Jpeg2000ReaderProcessParams Jpeg2000ReaderPlugin::getProcessParams(const OfxTime time)
{
	if (varyOnTime() && ( time < getFirstTime() || time > getLastTime() ) )
	{
		BOOST_THROW_EXCEPTION( exception::BadIndex()
			<< exception::user("Time value outside bounds.") );
	}
	Jpeg2000ReaderProcessParams params;

	params._paramFilepath = getAbsoluteFilenameAt(time);
	return params;
}

void Jpeg2000ReaderPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( paramName == kParamReaderFilename )
	{
		_reader.close();
		_fileInfos._failed = true;
	}
	ReaderPlugin::changedParam(args, paramName);
}

bool Jpeg2000ReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	FileInfo fileInfo = retrieveFileInfo( args.time );
	if ( fileInfo._failed )
	{
		TUTTLE_COUT_ERROR( "Jpeg2000ReaderPlugin::render: file info failed." );
		return false;
	}

	rod.x1 = 0;
	rod.x2 = fileInfo._width;
	rod.y1 = 0;
	rod.y2 = fileInfo._height;

	return true;
}

void Jpeg2000ReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	ReaderPlugin::getClipPreferences( clipPreferences );

	FileInfo fileInfo = retrieveFileInfo( getFirstTime() );
	if ( fileInfo._failed )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
			<< exception::user( "Unable to read file infos." ) );
	}

	if( getExplicitConversion() == eParamReaderExplicitConversionAuto )
	{
		clipPreferences.setPixelAspectRatio( *_clipDst, 1.0 );
		switch( fileInfo._components )
		{
			case 1:
				clipPreferences.setClipComponents( *_clipDst, OFX::ePixelComponentAlpha );
				break;
			case 3:
			case 4:
				clipPreferences.setClipComponents( *_clipDst, OFX::ePixelComponentRGBA );
				break;
			default:
			{
				BOOST_THROW_EXCEPTION( exception::ImageFormat()
					<< exception::user() + "Unexpected number of channels (" + fileInfo._components + ")" );
			}
		}

		clipPreferences.setClipBitDepth( *_clipDst, fileInfo._precisionType );
	}
	else // if we explicitly specify which conversion we want
	{
		clipPreferences.setClipBitDepth( *_clipDst, getOfxExplicitConversion() );
		clipPreferences.setClipComponents( *_clipDst, OFX::ePixelComponentRGBA );
		clipPreferences.setPixelAspectRatio( *_clipDst, 1.0 );
	}
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void Jpeg2000ReaderPlugin::render( const OFX::RenderArguments &args )
{
	if( retrieveFileInfo(args.time)._failed )
	{
		TUTTLE_COUT_ERROR( "Jpeg2000ReaderPlugin::render: file info failed." );
		return;
	}
	// Full image decoding
	_reader.decode();

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
				Jpeg2000ReaderProcess<rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				Jpeg2000ReaderProcess<rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				Jpeg2000ReaderProcess<rgba32f_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				TUTTLE_COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				TUTTLE_COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}
	else if( dstComponents == OFX::ePixelComponentAlpha )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				Jpeg2000ReaderProcess<gray8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				Jpeg2000ReaderProcess<gray16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				Jpeg2000ReaderProcess<gray32f_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				TUTTLE_COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				TUTTLE_COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}
	else
	{
		TUTTLE_COUT_FATALERROR( dstComponents << " not recognize." );
	}
}

Jpeg2000ReaderPlugin::FileInfo Jpeg2000ReaderPlugin::retrieveFileInfo( const OfxTime time )
{
	if( !_fileInfos._failed )
	{
		if( time == _fileInfos._time && _reader.imageReady() )
		{
			return _fileInfos;
		}

		if( _reader.imageReady() )
		{
			_reader.close();
		}
	}

	// Open new source
	try
	{
		_reader.open( getAbsoluteFilenameAt(time) );
	}
	catch(std::exception & e)
	{
		_fileInfos._failed = true;
		BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatFailed, e.what()) );
	}
	_fileInfos._failed = false;
	// No choice if we want to get
	// the image dimensions later
	try
	{
		_reader.decode(true);
	}
	catch(std::exception & e)
	{
		_fileInfos._failed = true;
		BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatFailed, e.what()) );
	}

	if( !_reader.componentsConform() )
	{
		_fileInfos._failed = true;
		BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrImageFormat, "Jpeg2000 image components aren't conform.") );
	}

	_fileInfos._time = time;
	_fileInfos._width = _reader.width();
	_fileInfos._height = _reader.height();
	_fileInfos._components = _reader.components();
	_fileInfos._precision = _reader.precision();

	switch( _fileInfos._precision )
	{
		case 8:
			_fileInfos._precisionType = OFX::eBitDepthUByte;
			break;
		case 12:
		case 16:
			_fileInfos._precisionType = OFX::eBitDepthUShort;
			break;
		case 32:
			_fileInfos._precisionType = OFX::eBitDepthFloat;
			break;
		default:
			_fileInfos._failed = true;
			BOOST_THROW_EXCEPTION( OFX::Exception::Suite( kOfxStatErrImageFormat, std::string("Bit depth not handled ! (") + boost::lexical_cast<std::string>(_reader.precision()) + ")" ) );
	}

	return _fileInfos;
}

}
}
}
}
