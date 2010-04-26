#include "Jpeg2000ReaderPlugin.hpp"
#include "Jpeg2000ReaderProcess.hpp"
#include "Jpeg2000ReaderDefinitions.hpp"
#include "tuttle/plugin/context/WriterDefinition.hpp"

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
	memset(&_fileInfos, 0, sizeof(_fileInfos));
	_fileInfos._failed = true;
}

Jpeg2000ReaderProcessParams Jpeg2000ReaderPlugin::getProcessParams(const OfxTime time)
{
	OfxRangeI range = _filePattern.getRange();
	if (varyOnTime() && (time < range.min || time > range.max) )
	{
		throw(OFX::Exception::Suite(kOfxStatErrBadIndex, "Time value outside bounds."));
	}
	Jpeg2000ReaderProcessParams params;

	params._filepath = _filePattern.getFilenameAt(time);
	return params;
}

J2KReader & Jpeg2000ReaderPlugin::getReader()
{
	return _reader;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void Jpeg2000ReaderPlugin::render( const OFX::RenderArguments &args )
{
	if( retrieveFileInfo(args.time)._failed )
	{
		COUT_ERROR( "Jpeg2000ReaderPlugin::render: file info failed." );
		return;
	}

	// instantiate the render code based on the pixel depth of the dst clip
	OFX::BitDepthEnum dstBitDepth         = this->_dstClip->getPixelDepth();
	OFX::PixelComponentEnum dstComponents = this->_dstClip->getPixelComponents();
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

void Jpeg2000ReaderPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
	if( paramName == kTuttlePluginReaderParamFilename )
	{
		_reader.close();
		_fileInfos._failed = 1;
	}
	ReaderPlugin::changedParam(args, paramName);
}

bool Jpeg2000ReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	FileInfo fileInfo = retrieveFileInfo( args.time );

	rod.x1 = 0;
	rod.x2 = fileInfo._width;
	rod.y1 = 0;
	rod.y2 = fileInfo._height;

	return true;
}

void Jpeg2000ReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	try
	{
		_filePattern.reset(_filepath->getValue(), true);

		// If pattern detected (frame varying on time)
		clipPreferences.setOutputFrameVarying( varyOnTime() );

		FileInfo fileInfo = retrieveFileInfo( _filePattern.getRange().min );

		// If we explicitly specify which conversion we want
		if ( getParamExplicitConversion() != OFX::eBitDepthNone )
		{
			clipPreferences.setClipBitDepth( *_dstClip, getParamExplicitConversion() );
			clipPreferences.setClipComponents( *_dstClip, OFX::ePixelComponentRGBA );
			clipPreferences.setPixelAspectRatio( *_dstClip, 1.0 );
		}
		else
		{
			clipPreferences.setPixelAspectRatio( *_dstClip, 1.0 );
			switch(fileInfo._components)
			{
				case 3:
				case 4:
					clipPreferences.setClipComponents( *_dstClip, OFX::ePixelComponentRGBA );
					break;
				case 1:
					clipPreferences.setClipComponents( *_dstClip, OFX::ePixelComponentAlpha );
					break;
				default:
				{
					throw( OFX::Exception::Suite( kOfxStatErrImageFormat, "Unexpected number of channels, settings pixel component to custom." ) );
				}
			}

			clipPreferences.setClipBitDepth( *_dstClip, fileInfo._precisionType );
		}
	}
	catch(...)
	{
	}
}

Jpeg2000ReaderPlugin::FileInfo Jpeg2000ReaderPlugin::retrieveFileInfo( const OfxTime time )
{
	if( time == _fileInfos._time && _reader.imageReady() )
		return _fileInfos;

	if( _reader.imageReady() )
	{
		_reader.close();
	}

	_fileInfos._failed = false;

	// Open new source
	try
	{
		_reader.open( _filePattern.getFilenameAt(time) );
	}
	catch(std::exception & e)
	{
		_fileInfos._failed = true;
		throw( OFX::Exception::Suite( kOfxStatFailed, e.what()) );
	}
	// No choice if we want to get
	// the image dimensions later
	try
	{
		_reader.decode();
	}
	catch(std::exception & e)
	{
		_fileInfos._failed = true;
		throw( OFX::Exception::Suite( kOfxStatFailed, e.what()) );
	}
	if( !_reader.componentsConform() )
	{
		_fileInfos._failed = true;
		throw( OFX::Exception::Suite( kOfxStatErrImageFormat, "Jpeg2000 image components aren't conform.") );
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
			throw( OFX::Exception::Suite( kOfxStatErrImageFormat, std::string("Bit depth not handled ! (") + boost::lexical_cast<std::string>(_reader.precision()) + ")" ) );
	}

	return _fileInfos;
}

}
}
}
}
