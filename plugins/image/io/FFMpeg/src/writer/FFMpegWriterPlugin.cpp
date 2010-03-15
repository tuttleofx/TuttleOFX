#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterProcess.hpp"
#include "FFMpegWriterDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/filesystem.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

using namespace boost::gil;
using namespace boost::filesystem;

FFMpegWriterPlugin::FFMpegWriterPlugin( OfxImageEffectHandle handle )
: ImageEffect( handle )
{
	// We want to render a sequence
	setSequentialRender( true );

    _srcClip = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _dstClip = fetchClip( kOfxImageEffectOutputClipName );
	_filepath = fetchStringParam( kOutputFilename );
}

OFX::Clip* FFMpegWriterPlugin::getSrcClip( ) const
{
    return _srcClip;
}

OFX::Clip* FFMpegWriterPlugin::getDstClip( ) const
{
    return _dstClip;
}

FFMpegParams FFMpegWriterPlugin::getParams() const
{
	FFMpegParams params;
	_filepath->getValue( params._filepath );
	return params;
}

VideoFFmpegWriter & FFMpegWriterPlugin::getWriter()
{
	return _writer;
}


/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FFMpegWriterPlugin::render( const OFX::RenderArguments &args )
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
                FFMpegProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                FFMpegProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                FFMpegProcess<rgba32f_view_t> p( *this );
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
    else if( dstComponents == OFX::ePixelComponentAlpha )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                FFMpegProcess<gray8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                FFMpegProcess<gray16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                FFMpegProcess<gray32f_view_t> p( *this );
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

void FFMpegWriterPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kFFMpegHelpButton )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kFFMpegHelpString );
    }
	else if( paramName == kOutputFilename )
	{
		std::string sFilepath;
		_filepath->getValue( sFilepath );
		// Check if exist
		if( exists( sFilepath ) )
		{
			// Close last opened file
			if ( _openedSource.get() )
			{
				_writer.finish();
			}
			// Open new source
			_openedSource.reset( new std::string( sFilepath ) );
			_writer.filename(sFilepath);
		}
	}
}

void FFMpegWriterPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
}

void FFMpegWriterPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{
	_writer.finish();
}

void FFMpegWriterPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	if ( _openedSource.get() )
	{
		clipPreferences.setClipComponents( *_dstClip, OFX::ePixelComponentRGBA );
		clipPreferences.setClipBitDepth( *_dstClip, OFX::eBitDepthUByte );
		clipPreferences.setPixelAspectRatio( *_dstClip, _writer.aspectRatio() );
//		clipPreferences.setOutputFrameRate( _writer.fps() );
/*
		// Setup fielding
		switch( _writer.interlacment() )
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
*/
	}
}

}
}
}
}
