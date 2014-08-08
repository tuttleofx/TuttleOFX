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


static const std::string kParamFormat                    = "format";
static const std::string kParamVideoCodec                = "videoCodec";
static const std::string kParamAudioCodec                = "audioCodec";

static const std::string kParamFormatGroup               = "formatGroup";
static const std::string kParamFormatDetailledGroup      = "formatDetailledGroup";
static const std::string kParamVideoGroup                = "videoGroup";
static const std::string kParamVideoDetailledGroup       = "videoDetailledGroup";
static const std::string kParamAudioGroup                = "audioGroup";
static const std::string kParamAudioDetailledGroup       = "audioDetailledGroup";
static const std::string kParamMetaGroup                 = "metaGroup";
static const std::string kParamMetaDetailledGroup        = "metaDetailledGroup";
static const std::string kParamAboutGroup                = "aboutGroup";

static const std::string kParamMainPreset                = "mainPreset";

static const std::string kParamFormatPreset              = "formatPreset";
static const std::string kParamVideoPreset               = "videoPreset";
static const std::string kParamAudioMainPreset           = "audioMainPreset";

static const std::string kParamUseCustomFps              = "useCustomFps";
static const std::string kParamCustomFps                 = "customFps";
static const std::string kParamVideoCodecPixelFmt        = "videoPixelFormat";
static const std::string kParamAudioCodecSampleFmt       = "audioSampleFormat";

static const std::string kParamFormatCustomGroup         = "formatCustomGroup";
static const std::string kParamVideoCustomGroup          = "videoCustomGroup";
static const std::string kParamAudioCustomGroup          = "audioCustomGroup";

static const std::string kParamAudioNbStream             = "audioNbStream";

static const std::string kParamAudioSubGroup             = "audioSubGroup";
static const std::string kParamAudioSilent               = "audioSilent";
static const std::string kParamAudioFilePath             = "audioFilePath";
static const std::string kParamAudioSelectStream         = "audioSelectStream";
static const std::string kParamAudioStreamIndex          = "audioStreamIndex";
static const std::string kParamAudioTranscodeStream      = "audioTranscodeStream";
static const std::string kParamAudioPreset               = "audioPreset";
static const std::string kParamAudioSelectChannel        = "audioSelectChannel";
static const std::string kParamAudioChannelIndex         = "audioChannelIndex";

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
static const std::string kParamAboutLicence        = "licence";
static const std::string kParamAboutVersion        = "version";

}
}
}
}

#endif
