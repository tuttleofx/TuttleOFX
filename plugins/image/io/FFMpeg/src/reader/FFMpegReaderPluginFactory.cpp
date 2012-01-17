#include "FFMpegReaderPluginFactory.hpp"
#include "FFMpegReaderPlugin.hpp"
#include "FFMpegReaderDefinitions.hpp"

#include <tuttle/plugin/context/ReaderPluginFactory.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace reader {

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void FFMpegReaderPluginFactory::describe( OFX::ImageEffectDescriptor& desc )
{
    desc.setLabels( "TuttleFfmpegReader", "FfmpegReader",
                    "Ffmpeg video reader" );
    desc.setPluginGrouping( "tuttle/image/io" );

    desc.setDescription( "Video reader based on FFMpeg library" );

    // add the supported contexts
    desc.addSupportedContext( OFX::eContextReader );
    desc.addSupportedContext( OFX::eContextGeneral );

    // add supported pixel depths
    desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    desc.addSupportedBitDepth( OFX::eBitDepthUShort );
    desc.addSupportedBitDepth( OFX::eBitDepthFloat );

    // add supported extensions
    desc.addSupportedExtension( "4xm" );
    desc.addSupportedExtension( "iff" );
    desc.addSupportedExtension( "iss" );
    desc.addSupportedExtension( "mtv" );
    desc.addSupportedExtension( "roq" );
    desc.addSupportedExtension( "aac" );
    desc.addSupportedExtension( "ac3" );
    desc.addSupportedExtension( "aea" );
    desc.addSupportedExtension( "aiff" );
    desc.addSupportedExtension( "alaw" );
    desc.addSupportedExtension( "alsa" );
    desc.addSupportedExtension( "amr" );
    desc.addSupportedExtension( "anm" );
    desc.addSupportedExtension( "apc" );
    desc.addSupportedExtension( "ape" );
    desc.addSupportedExtension( "applehttp" );
    desc.addSupportedExtension( "asf" );
    desc.addSupportedExtension( "ass" );
    desc.addSupportedExtension( "au" );
    desc.addSupportedExtension( "avi" );
    desc.addSupportedExtension( "avs" );
    desc.addSupportedExtension( "bethsoftvid" );
    desc.addSupportedExtension( "bfi" );
    desc.addSupportedExtension( "bink" );
    desc.addSupportedExtension( "c93" );
    desc.addSupportedExtension( "caf" );
    desc.addSupportedExtension( "cavsvideo" );
    desc.addSupportedExtension( "cdg" );
    desc.addSupportedExtension( "daud" );
    desc.addSupportedExtension( "dfa" );
    desc.addSupportedExtension( "dirac" );
    desc.addSupportedExtension( "dnxhd" );
    desc.addSupportedExtension( "dsicin" );
    desc.addSupportedExtension( "dts" );
    desc.addSupportedExtension( "dv" );
    desc.addSupportedExtension( "dv1394" );
    desc.addSupportedExtension( "dxa" );
    desc.addSupportedExtension( "ea" );
    desc.addSupportedExtension( "ea_cdata" );
    desc.addSupportedExtension( "eac3" );
    desc.addSupportedExtension( "f32be" );
    desc.addSupportedExtension( "f32le" );
    desc.addSupportedExtension( "f64be" );
    desc.addSupportedExtension( "f64le" );
    desc.addSupportedExtension( "fbdev" );
    desc.addSupportedExtension( "ffm" );
    desc.addSupportedExtension( "ffmetadata" );
    desc.addSupportedExtension( "film_cpk" );
    desc.addSupportedExtension( "filmstrip" );
    desc.addSupportedExtension( "flac" );
    desc.addSupportedExtension( "flic" );
    desc.addSupportedExtension( "flv" );
    desc.addSupportedExtension( "g722" );
    desc.addSupportedExtension( "gsm" );
    desc.addSupportedExtension( "gxf" );
    desc.addSupportedExtension( "h261" );
    desc.addSupportedExtension( "h263" );
    desc.addSupportedExtension( "h264" );
    desc.addSupportedExtension( "idcin" );
    desc.addSupportedExtension( "image2" );
    desc.addSupportedExtension( "ingenient" );
    desc.addSupportedExtension( "ipmovie" );
    desc.addSupportedExtension( "iv8" );
    desc.addSupportedExtension( "ivf" );
    desc.addSupportedExtension( "jack" );
    desc.addSupportedExtension( "jv" );
    desc.addSupportedExtension( "libdc1394" );
    desc.addSupportedExtension( "lmlm4" );
    desc.addSupportedExtension( "lxf" );
    desc.addSupportedExtension( "m4v" );
    desc.addSupportedExtension( "matroska" );
    desc.addSupportedExtension( "webm" );
    desc.addSupportedExtension( "microdvd" );
    desc.addSupportedExtension( "mjpeg" );
    desc.addSupportedExtension( "mlp" );
    desc.addSupportedExtension( "mm" );
    desc.addSupportedExtension( "mmf" );
    desc.addSupportedExtension( "mov" );
    desc.addSupportedExtension( "mp4" );
    desc.addSupportedExtension( "m4a" );
    desc.addSupportedExtension( "3gp" );
    desc.addSupportedExtension( "3g2" );
    desc.addSupportedExtension( "mj2" );
    desc.addSupportedExtension( "mp3" );
    desc.addSupportedExtension( "mpc" );
    desc.addSupportedExtension( "mpc8" );
    desc.addSupportedExtension( "mpeg" );
    desc.addSupportedExtension( "mpegts" );
    desc.addSupportedExtension( "mpegtsraw" );
    desc.addSupportedExtension( "mpegvideo" );
    desc.addSupportedExtension( "msnwctcp" );
    desc.addSupportedExtension( "mulaw" );
    desc.addSupportedExtension( "mvi" );
    desc.addSupportedExtension( "mxf" );
    desc.addSupportedExtension( "mxg" );
    desc.addSupportedExtension( "nc" );
    desc.addSupportedExtension( "nsv" );
    desc.addSupportedExtension( "nut" );
    desc.addSupportedExtension( "nuv" );
    desc.addSupportedExtension( "ogg" );
    desc.addSupportedExtension( "oma" );
    desc.addSupportedExtension( "oss" );
    desc.addSupportedExtension( "pmp" );
    desc.addSupportedExtension( "psxstr" );
    desc.addSupportedExtension( "pva" );
    desc.addSupportedExtension( "qcp" );
    desc.addSupportedExtension( "r3d" );
    desc.addSupportedExtension( "rawvideo" );
    desc.addSupportedExtension( "rl2" );
    desc.addSupportedExtension( "rm" );
    desc.addSupportedExtension( "rpl" );
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
    desc.addSupportedExtension( "sdp" );
    desc.addSupportedExtension( "shn" );
    desc.addSupportedExtension( "siff" );
    desc.addSupportedExtension( "smk" );
    desc.addSupportedExtension( "sol" );
    desc.addSupportedExtension( "sox" );
    desc.addSupportedExtension( "spdif" );
    desc.addSupportedExtension( "srt" );
    desc.addSupportedExtension( "swf" );
    desc.addSupportedExtension( "thp" );
    desc.addSupportedExtension( "tiertexseq" );
    desc.addSupportedExtension( "tmv" );
    desc.addSupportedExtension( "truehd" );
    desc.addSupportedExtension( "tta" );
    desc.addSupportedExtension( "tty" );
    desc.addSupportedExtension( "txd" );
    desc.addSupportedExtension( "u16be" );
    desc.addSupportedExtension( "u16le" );
    desc.addSupportedExtension( "u24be" );
    desc.addSupportedExtension( "u24le" );
    desc.addSupportedExtension( "u32be" );
    desc.addSupportedExtension( "u32le" );
    desc.addSupportedExtension( "u8" );
    desc.addSupportedExtension( "vc1" );
    desc.addSupportedExtension( "video4linux" );
    desc.addSupportedExtension( "video4linux2" );
    desc.addSupportedExtension( "vmd" );
    desc.addSupportedExtension( "voc" );
    desc.addSupportedExtension( "vqf" );
    desc.addSupportedExtension( "w64" );
    desc.addSupportedExtension( "wav" );
    desc.addSupportedExtension( "wc3movie" );
    desc.addSupportedExtension( "wsaud" );
    desc.addSupportedExtension( "wsvqa" );
    desc.addSupportedExtension( "wtv" );
    desc.addSupportedExtension( "wv" );
    desc.addSupportedExtension( "x11grab" );
    desc.addSupportedExtension( "xa" );
    desc.addSupportedExtension( "xwma" );
    desc.addSupportedExtension( "yop" );

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
void FFMpegReaderPluginFactory::describeInContext( OFX::ImageEffectDescriptor& desc,
                                                   OFX::EContext               context )
{
    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip( kOfxImageEffectOutputClipName );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGBA );
    dstClip->addSupportedComponent( OFX::ePixelComponentRGB );
    dstClip->addSupportedComponent( OFX::ePixelComponentAlpha );
    dstClip->setSupportsTiles( kSupportTiles );

    describeReaderParamsInContext( desc, context );
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* FFMpegReaderPluginFactory::createInstance( OfxImageEffectHandle handle,
                                                             OFX::EContext        context )
{
    return new FFMpegReaderPlugin( handle );
}

}
}
}
}
