#include "FFMpegReaderPlugin.hpp"
#include "FFMpegReaderProcess.hpp"
#include "FFMpegReaderDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace reader {

using namespace boost::gil;
using namespace boost::filesystem;

FFMpegReaderPlugin::FFMpegReaderPlugin( OfxImageEffectHandle handle )
: ImageEffect( handle )
{
	// We want to render a sequence
	setSequentialRender( true );

    _dstClip = fetchClip( kOfxImageEffectOutputClipName );
	_filepath = fetchStringParam( kFilename );
}

FFMpegReaderParams FFMpegReaderPlugin::getParams() const
{
	FFMpegReaderParams params;
	_filepath->getValue( params._filepath );
	return params;
}

VideoFFmpegReader & FFMpegReaderPlugin::getReader()
{
	return _reader;
}


/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FFMpegReaderPlugin::render( const OFX::RenderArguments &args )
{
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::BitDepthEnum dstBitDepth = _dstClip->getPixelDepth( );
    OFX::PixelComponentEnum dstComponents = _dstClip->getPixelComponents( );

    // do the rendering
    if( dstComponents == OFX::ePixelComponentRGBA )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                FFMpegReaderProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                FFMpegReaderProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                FFMpegReaderProcess<rgba32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthNone :
                COUT_FATALERROR( "BitDepthNone not recognized." );
                return;
            case OFX::eBitDepthCustom :
                COUT_FATALERROR( "BitDepthCustom not recognized." );
                return;
        }
    }
    else if( dstComponents == OFX::ePixelComponentAlpha )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                FFMpegReaderProcess<gray8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                FFMpegReaderProcess<gray16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                FFMpegReaderProcess<gray32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthNone :
                COUT_FATALERROR( "BitDepthNone not recognize." );
                return;
            case OFX::eBitDepthCustom :
                COUT_FATALERROR( "BitDepthCustom not recognize." );
                return;
        }
    }
}

void FFMpegReaderPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kFFMpegHelpButton )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kFFMpegHelpString );
    }
	else if( paramName == kFilename )
	{
		std::string sFilepath;
		_filepath->getValue( sFilepath );
		// Check if exist
		if( exists( sFilepath ) )
		{
			// Close last opened file
			if ( _openedSource.get() )
				_reader.close();
			// Open new source
			_openedSource.reset( new std::string( sFilepath ) );
			_reader.open( sFilepath.c_str() );
		}
	}
}

bool FFMpegReaderPlugin::getTimeDomain( OfxRangeD& range )
{
	// Source opened ?
	if ( _openedSource.get() )
	{
		range.min = 0.0;
		range.max = (double)_reader.nbFrames();
	}
}

bool FFMpegReaderPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	rod.x1 = 0;
	rod.x2 = _reader.width();
	rod.y1 = 0;
	rod.y2 = _reader.height();
	return true;
}

void FFMpegReaderPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
}

void FFMpegReaderPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{
	_reader.close();
}

void FFMpegReaderPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	if ( _openedSource.get() )
	{
		clipPreferences.setClipComponents( *_dstClip, OFX::ePixelComponentRGBA );
		clipPreferences.setClipBitDepth( *_dstClip, OFX::eBitDepthUByte );
		clipPreferences.setPixelAspectRatio( *_dstClip, _reader.aspectRatio() );
		clipPreferences.setOutputFrameRate( _reader.fps() );

		// Setup fielding
		switch( _reader.interlacment() )
		{
			case  eInterlacmentNone:
			{
				clipPreferences.setOutputFielding( OFX::eFieldNone );
				break;
			}
			case  eInterlacmentUpper:
			{
				clipPreferences.setOutputFielding( OFX::eFieldUpper );
				break;
			}
			case  eInterlacmentLower:
			{
				clipPreferences.setOutputFielding( OFX::eFieldLower );
				break;
			}
		}
	}
}

}
}
}
}
