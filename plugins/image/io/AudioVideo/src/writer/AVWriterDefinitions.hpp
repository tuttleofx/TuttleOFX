#ifndef _TUTTLE_PLUGIN_AV_WRITER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_AV_WRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace av {
namespace writer {

enum ETuttlePluginBitDepth
{
	eTuttlePluginBitDepth8 = 0
};


static const std::string kParamFormat                    = "f_format";
static const std::string kParamVideoCodec                = "v_codec";
static const std::string kParamAudioCodec                = "a_codec";

static const std::string kParamFormatGroup               = "f_group";
static const std::string kParamFormatDetailledGroup      = "f_detailledGroup";
static const std::string kParamVideoGroup                = "v_group";
static const std::string kParamVideoDetailledGroup       = "v_detailledGroup";
static const std::string kParamAudioGroup                = "a_group";
static const std::string kParamAudioDetailledGroup       = "a_detailledGroup";
static const std::string kParamMetaGroup                 = "m_group";
static const std::string kParamMetaDetailledGroup        = "m_detailledGroup";
static const std::string kParamAboutGroup                = "aboutGroup";

static const std::string kParamMainPreset                = "mainPreset";

static const std::string kParamFormatPreset              = "f_preset";
static const std::string kParamVideoPreset               = "v_preset";
static const std::string kParamAudioMainPreset           = "a_mainPreset";

static const std::string kParamUseCustomFps              = "v_useCustomFps";
static const std::string kParamCustomFps                 = "v_customFps";
static const std::string kParamVideoCodecPixelFmt        = "v_pixelFormat";
static const std::string kParamAudioCodecSampleFmt       = "a_sampleFormat";

static const std::string kParamFormatCustomGroup         = "f_customGroup";
static const std::string kParamVideoCustomGroup          = "v_customGroup";
static const std::string kParamAudioCustomGroup          = "a_customGroup";

static const std::string kParamAudioNbStream             = "a_nbStream";

static const std::string kParamAudioSubGroup             = "a_subGroup";
static const std::string kParamAudioSilent               = "a_silent";
static const std::string kParamAudioFilePath             = "a_filePath";
static const std::string kParamAudioSelectStream         = "a_selectStream";
static const std::string kParamAudioStreamIndex          = "a_streamIndex";
static const std::string kParamAudioTranscodeStream      = "a_transcodeStream";
static const std::string kParamAudioPreset               = "a_preset";
static const std::string kParamAudioSelectChannel        = "a_selectChannel";
static const std::string kParamAudioChannelIndex         = "a_channelIndex";
static const std::string kParamAudioOffset               = "a_offset";

static const size_t maxNbAudioStream                     = 16;

static const std::string kParamMetaAlbum           = "m_album";
static const std::string kParamMetaAlbumArtist     = "m_album_artist";
static const std::string kParamMetaArtist          = "m_artist";
static const std::string kParamMetaComment         = "m_comment";
static const std::string kParamMetaComposer        = "m_composer";
static const std::string kParamMetaCopyright       = "m_copyright";
static const std::string kParamMetaCreationTime    = "m_creation_time";
static const std::string kParamMetaDate            = "m_date";
static const std::string kParamMetaDisc            = "m_disc";
static const std::string kParamMetaEncoder         = "m_encoder";
static const std::string kParamMetaEncodedBy       = "m_encoded_by";
static const std::string kParamMetaFilename        = "m_filename";
static const std::string kParamMetaGenre           = "m_genre";
static const std::string kParamMetaLanguage        = "m_language";
static const std::string kParamMetaPerformer       = "m_performer";
static const std::string kParamMetaPublisher       = "m_publisher";
static const std::string kParamMetaServiceName     = "m_service_name";
static const std::string kParamMetaServiceProvider = "m_service_provider";
static const std::string kParamMetaTitle           = "m_title";
static const std::string kParamMetaTrack           = "m_track";
static const std::string kParamMetaVariantBitrate  = "m_variant_bitrate";

static const std::string kParamAboutLibName        = "libName";
static const std::string kParamAboutLicense        = "license";
static const std::string kParamAboutVersion        = "version";

}
}
}
}

#endif
