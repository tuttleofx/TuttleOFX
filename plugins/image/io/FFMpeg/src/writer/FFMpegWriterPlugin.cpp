#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterProcess.hpp"
#include "FFMpegWriterDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

using namespace boost::gil;

FFMpegWriterPlugin::FFMpegWriterPlugin( OfxImageEffectHandle handle )
: ImageEffect( handle )
{
	// We want to render a sequence
	setSequentialRender( true );

    _srcClip = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _dstClip = fetchClip( kOfxImageEffectOutputClipName );
	_filepath = fetchStringParam( kFilename );
	_format = fetchChoiceParam( kFormat );
	_formatLong = fetchChoiceParam( kFormatLong );
	_codec = fetchChoiceParam( kCodec );
	_codecLong = fetchChoiceParam( kCodecLong );
}

OFX::Clip* FFMpegWriterPlugin::getSrcClip( ) const
{
    return _srcClip;
}

OFX::Clip* FFMpegWriterPlugin::getDstClip( ) const
{
    return _dstClip;
}

FFMpegProcessParams FFMpegWriterPlugin::getProcessParams() const
{
	FFMpegProcessParams params;
	_filepath->getValue( params._filepath );
	_format->getValue( params._format );
	_codec->getValue( params._codec );
	return params;
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
                FFMpegWriterProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                FFMpegWriterProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                FFMpegWriterProcess<rgba32f_view_t> p( *this );
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
                FFMpegWriterProcess<gray8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                FFMpegWriterProcess<gray16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                FFMpegWriterProcess<gray32f_view_t> p( *this );
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
	else if( paramName == kFormatLong )
	{
		_format->setValue( _formatLong->getValue() );
	}
	else if( paramName == kFormat )
	{
		_formatLong->setValue( _format->getValue() );
	}
	else if( paramName == kCodecLong )
	{
		_codec->setValue( _codecLong->getValue() );
	}
	else if( paramName == kCodec )
	{
		_codecLong->setValue( _codec->getValue() );
	}
}

void FFMpegWriterPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
}

void FFMpegWriterPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{
//	_writer.finish();
}

void FFMpegWriterPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	if ( _openedSource.get() )
	{
		clipPreferences.setClipComponents( *_dstClip, OFX::ePixelComponentRGBA );
		clipPreferences.setClipBitDepth( *_dstClip, OFX::eBitDepthUByte );
//		clipPreferences.setPixelAspectRatio( *_dstClip, _writer.aspectRatio() );
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
