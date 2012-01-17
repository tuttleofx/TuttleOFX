#include "FFMpegWriterPluginFactory.hpp"
#include "FFMpegWriterPlugin.hpp"
#include "FFMpegWriterDefinitions.hpp"

#include <ffmpeg/VideoFFmpegWriter.hpp>
#include <tuttle/plugin/context/WriterPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void FFMpegWriterPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
    desc.setLabels( "TuttleFfmpegWriter", "FfmpegWriter",
                    "Ffmpeg video writer" );
    desc.setPluginGrouping( "tuttle/image/io" );

    // add the supported contexts
    desc.addSupportedContext( OFX::eContextWriter );
    desc.addSupportedContext( OFX::eContextGeneral );

    // add supported pixel depths
    desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    desc.addSupportedBitDepth( OFX::eBitDepthUShort );
    desc.addSupportedBitDepth( OFX::eBitDepthFloat );

    // add supported extensions
    desc.addSupportedExtension( "3g2" );
    desc.addSupportedExtension( "3gp" );
    desc.addSupportedExtension( "roq" );
    desc.addSupportedExtension( "a64" );
    desc.addSupportedExtension( "ac3" );
    desc.addSupportedExtension( "adts" );
    desc.addSupportedExtension( "aiff" );
    desc.addSupportedExtension( "alaw" );
    desc.addSupportedExtension( "alsa" );
    desc.addSupportedExtension( "amr" );
    desc.addSupportedExtension( "asf" );
    desc.addSupportedExtension( "asf_stream" );
    desc.addSupportedExtension( "ass" );
    desc.addSupportedExtension( "au" );
    desc.addSupportedExtension( "avi" );
    desc.addSupportedExtension( "avm2" );
    desc.addSupportedExtension( "cavsvideo" );
    desc.addSupportedExtension( "crc" );
    desc.addSupportedExtension( "daud" );
    desc.addSupportedExtension( "dirac" );
    desc.addSupportedExtension( "dnxhd" );
    desc.addSupportedExtension( "dts" );
    desc.addSupportedExtension( "dv" );
    desc.addSupportedExtension( "dvd" );
    desc.addSupportedExtension( "eac3" );
    desc.addSupportedExtension( "f32be" );
    desc.addSupportedExtension( "f32le" );
    desc.addSupportedExtension( "f64be" );
    desc.addSupportedExtension( "f64le" );
    desc.addSupportedExtension( "ffm" );
    desc.addSupportedExtension( "ffmetadata" );
    desc.addSupportedExtension( "filmstrip" );
    desc.addSupportedExtension( "flac" );
    desc.addSupportedExtension( "flv" );
    desc.addSupportedExtension( "framecrc" );
    desc.addSupportedExtension( "framemd5" );
    desc.addSupportedExtension( "g722" );
    desc.addSupportedExtension( "gif" );
    desc.addSupportedExtension( "gxf" );
    desc.addSupportedExtension( "h261" );
    desc.addSupportedExtension( "h263" );
    desc.addSupportedExtension( "h264" );
    desc.addSupportedExtension( "image2" );
    desc.addSupportedExtension( "ipod" );
    desc.addSupportedExtension( "ivf" );
    desc.addSupportedExtension( "m4v" );
    desc.addSupportedExtension( "matroska" );
    desc.addSupportedExtension( "md5" );
    desc.addSupportedExtension( "microdvd" );
    desc.addSupportedExtension( "mjpeg" );
    desc.addSupportedExtension( "mlp" );
    desc.addSupportedExtension( "mmf" );
    desc.addSupportedExtension( "mov" );
    desc.addSupportedExtension( "mp3" );
    desc.addSupportedExtension( "mp3" );
    desc.addSupportedExtension( "mp4" );
    desc.addSupportedExtension( "mpeg" );
    desc.addSupportedExtension( "mpeg1video" );
    desc.addSupportedExtension( "mpeg2video" );
    desc.addSupportedExtension( "mpegts" );
    desc.addSupportedExtension( "mpjpeg" );
    desc.addSupportedExtension( "mulaw" );
    desc.addSupportedExtension( "mxf" );
    desc.addSupportedExtension( "mxf_d10" );
    desc.addSupportedExtension( "null" );
    desc.addSupportedExtension( "nut" );
    desc.addSupportedExtension( "ogg" );
    desc.addSupportedExtension( "oss" );
    desc.addSupportedExtension( "psp" );
    desc.addSupportedExtension( "rawvideo" );
    desc.addSupportedExtension( "rcv" );
    desc.addSupportedExtension( "rm" );
    desc.addSupportedExtension( "rso" );
    desc.addSupportedExtension( "rtp" );
    desc.addSupportedExtension( "rtsp" );
    desc.addSupportedExtension( "s16be" );
    desc.addSupportedExtension( "s16le" );
    desc.addSupportedExtension( "s24be" );
    desc.addSupportedExtension( "s24le" );
    desc.addSupportedExtension( "s32be" );
    desc.addSupportedExtension( "s32le" );
    desc.addSupportedExtension( "s8" );
    desc.addSupportedExtension( "sap" );
    desc.addSupportedExtension( "sox" );
    desc.addSupportedExtension( "spdif" );
    desc.addSupportedExtension( "srt" );
    desc.addSupportedExtension( "svcd" );
    desc.addSupportedExtension( "swf" );
    desc.addSupportedExtension( "truehd" );
    desc.addSupportedExtension( "u16be" );
    desc.addSupportedExtension( "u16le" );
    desc.addSupportedExtension( "u24be" );
    desc.addSupportedExtension( "u24le" );
    desc.addSupportedExtension( "u32be" );
    desc.addSupportedExtension( "u32le" );
    desc.addSupportedExtension( "u8" );
    desc.addSupportedExtension( "vcd" );
    desc.addSupportedExtension( "vob" );
    desc.addSupportedExtension( "voc" );
    desc.addSupportedExtension( "wav" );
    desc.addSupportedExtension( "webm" );

    // plugin flags
    desc.setRenderThreadSafety( OFX::eRenderInstanceSafe );
    desc.setHostFrameThreading( false );
    desc.setSupportsMultiResolution( false );
    desc.setSupportsMultipleClipDepths( true );
    desc.setSupportsTiles( kSupportTiles );
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void FFMpegWriterPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                   OFX::EContext               context )
{
    VideoFFmpegWriter writer;

    OFX::ClipDescriptor* srcClip = desc.defineClip( kOfxImageEffectSimpleSourceClipName );

    srcClip->addSupportedComponent( OFX::ePixelComponentRGBA );
    srcClip->addSupportedComponent( OFX::ePixelComponentRGB );
    srcClip->addSupportedComponent( OFX::ePixelComponentAlpha );
    srcClip->setSupportsTiles( kSupportTiles );

    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
    dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
    dstClip->setSupportsTiles( kSupportTiles );

    // Controls
    OFX::StringParamDescriptor* filename = desc.defineStringParam( kParamWriterFilename );
    filename->setLabel( "Filename" );
    filename->setStringType( OFX::eStringTypeFilePath );
    filename->setCacheInvalidation( OFX::eCacheInvalidateValueAll );

    OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam( kParamWriterBitDepth );
    bitDepth->setLabel( "Bit depth" );
    bitDepth->appendOption( kTuttlePluginBitDepth8 );
    bitDepth->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    bitDepth->setDefault( 0 );


    OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam( kParamFormat );
    std::vector<std::string>::const_iterator itLong;
    for( std::vector<std::string>::const_iterator itShort = writer.getFormatsShort().begin(),
        itLong = writer.getFormatsLong().begin(),
        itEnd = writer.getFormatsShort().end();
        itShort != itEnd;
        ++itShort,
        ++itLong )
    {
        std::string name = *itShort;
        name.resize ( 20,' ');
        name += *itLong ;
        format->appendOption( name );
    }
    format->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    format->setDefault( 25 );

    OFX::ChoiceParamDescriptor* codec = desc.defineChoiceParam( kParamCodec );
    for( std::vector<std::string>::const_iterator itShort = writer.getCodecsShort().begin(),
        itLong  = writer.getCodecsLong().begin(),
        itEnd = writer.getCodecsShort().end();
        itShort != itEnd;
        ++itShort,
        ++itLong )
    {
        std::string name = *itShort;
        name.resize ( 20,' ');
        name += *itLong ;
        codec->appendOption( name );
    }
    codec->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    codec->setDefault( CODEC_ID_HUFFYUV );

    OFX::IntParamDescriptor* bitrate = desc.defineIntParam( kParamBitrate );
    bitrate->setLabel( "Bitrate" );
    bitrate->setCacheInvalidation( OFX::eCacheInvalidateValueAll );
    bitrate->setDefault( 400000 );

    OFX::ChoiceParamDescriptor* colorSpace = desc.defineChoiceParam( kParamColorSpace );
    colorSpace->setLabel( "Color Space" );
    colorSpace->appendOption( kParamColorSpaceRgb         );
    colorSpace->appendOption( kParamColorSpaceBt709       );
    colorSpace->appendOption( kParamColorSpaceUnspecified );
    colorSpace->appendOption( kParamColorSpaceFcc         );
    colorSpace->appendOption( kParamColorSpaceBt470bg     );
    colorSpace->appendOption( kParamColorSpaceSmpte170m   );
    colorSpace->appendOption( kParamColorSpaceSmpte240m   );
    colorSpace->appendOption( kParamColorSpaceNb          );
    colorSpace->setDefault( AVCOL_SPC_UNSPECIFIED );

    OFX::ChoiceParamDescriptor* colorPrimaries = desc.defineChoiceParam( kParamColorPrimaries );
    colorPrimaries->setLabel( "Color Primaries" );
    colorPrimaries->appendOption( kParamColorPrimariesBt709       );
    colorPrimaries->appendOption( kParamColorPrimariesUnspecified );
    colorPrimaries->appendOption( kParamColorPrimariesBt470m      );
    colorPrimaries->appendOption( kParamColorPrimariesBt470bg     );
    colorPrimaries->appendOption( kParamColorPrimariesSmpte170m   );
    colorPrimaries->appendOption( kParamColorPrimariesSmpte240m   );
    colorPrimaries->appendOption( kParamColorPrimariesFilm        );
    colorPrimaries->appendOption( kParamColorPrimariesNb          );
    colorPrimaries->setDefault( AVCOL_PRI_UNSPECIFIED - 1 );

    OFX::ChoiceParamDescriptor* colorTRC = desc.defineChoiceParam( kParamColorTRC );
    colorTRC->setLabel( "Color Transfer Characteristic" );
    colorTRC->appendOption( kParamColorTRCBt709       );
    colorTRC->appendOption( kParamColorTRCUnspecified );
    colorTRC->appendOption( kParamColorTRCGamma22     );
    colorTRC->appendOption( kParamColorTRCGamma28     );
    colorTRC->appendOption( kParamColorTRCNb          );
    colorTRC->setDefault( AVCOL_TRC_UNSPECIFIED - 1 );


    OFX::ChoiceParamDescriptor* motionEstimation = desc.defineChoiceParam( kParamMotionEstimation );
    motionEstimation->setLabel( "Motion Estimation" );
    motionEstimation->appendOption( kParamMEZero  );
    motionEstimation->appendOption( kParamMEFull  );
    motionEstimation->appendOption( kParamMELog   );
    motionEstimation->appendOption( kParamMEPhods );
    motionEstimation->appendOption( kParamMEEpzs  );
    motionEstimation->appendOption( kParamMEXl    );
    motionEstimation->appendOption( kParamMEHex   );
    motionEstimation->appendOption( kParamMEUmh   );
    motionEstimation->appendOption( kParamMEIter  );
    motionEstimation->appendOption( kParamMETesa  );
    motionEstimation->setDefault ( ME_EPZS - 1 );

    describeWriterParamsInContext( desc, context );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* FFMpegWriterPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                             OFX::EContext        context )
{
    return new FFMpegWriterPlugin( handle );
}

}
}
}
}
