#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterProcess.hpp"
#include "FFMpegWriterDefinitions.hpp"

#include <ffmpeg/VideoFFmpegWriter.hpp>

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

    _clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _clipDst = fetchClip( kOfxImageEffectOutputClipName );
	_filepath = fetchStringParam( kParamFilename );
	_format = fetchChoiceParam( kParamFormat );
	_formatLong = fetchChoiceParam( kParamFormatLong );
	_codec = fetchChoiceParam( kParamCodec );
	_codecLong = fetchChoiceParam( kParamCodecLong );
	_bitRate = fetchIntParam( kParamBitrate );
	_paramRenderAlways   = fetchBooleanParam( kParamRenderAlways );
}

FFMpegProcessParams FFMpegWriterPlugin::getProcessParams() const
{
	FFMpegProcessParams params;
	_filepath->getValue( params._filepath );
	_format->getValue( params._format );
	_codec->getValue( params._codec );
	_bitRate->getValue( params._bitrate );
	return params;
}

void FFMpegWriterPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kParamFFMpegHelpButton )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kFFMpegHelpString );
    }
	else if( paramName == kParamFormatLong && args.reason == OFX::eChangeUserEdit  )
	{
		_format->setValue( _formatLong->getValue() );
	}
	else if( paramName == kParamFormat && args.reason == OFX::eChangeUserEdit )
	{
		_formatLong->setValue( _format->getValue() );
	}
	else if( paramName == kParamCodecLong && args.reason == OFX::eChangeUserEdit )
	{
		_codec->setValue( _codecLong->getValue() );
	}
	else if( paramName == kParamCodec && args.reason == OFX::eChangeUserEdit )
	{
		_codecLong->setValue( _codec->getValue() );
	}
}

bool FFMpegWriterPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, OfxTime& identityTime )
{
	if( OFX::getImageEffectHostDescription()->hostIsBackground )
		return false;

	if( _paramRenderAlways->getValue() )
		return false;

	identityClip = _clipSrc;
	identityTime = args.time;
	return true;
}

void FFMpegWriterPlugin::beginSequenceRender( const OFX::BeginSequenceRenderArguments& args )
{
	_writer.filename( _filepath->getValue() );
	_writer.setFormat( _format->getValue() );
	_writer.setCodec( _codec->getValue() );
	_writer.fps( _clipSrc->getFrameRate() );
	_writer.aspectRatio( _clipSrc->getPixelAspectRatio() );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FFMpegWriterPlugin::render( const OFX::RenderArguments &args )
{
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::BitDepthEnum dstBitDepth = _clipDst->getPixelDepth( );
    OFX::PixelComponentEnum dstComponents = _clipDst->getPixelComponents( );

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

void FFMpegWriterPlugin::endSequenceRender( const OFX::EndSequenceRenderArguments& args )
{
	_writer.finish();
}

}
}
}
}
