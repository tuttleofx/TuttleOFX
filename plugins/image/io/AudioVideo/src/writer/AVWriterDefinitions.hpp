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

static const std::string kParamMainPreset                = "mainPreset";

static const std::string kParamFormatPreset              = "formatPreset";
static const std::string kParamMainVideoPreset           = "mainVideoPreset";
static const std::string kParamMainAudioPreset           = "mainAudioPreset";

static const std::string kParamUseCustomFps              = "useCustomFps";
static const std::string kParamCustomFps                 = "customFps";
static const std::string kParamVideoCodecPixelFmt        = "videoPixelFormat";

static const std::string kParamAudioNbStream             = "audioNbStream";
static const std::string kParamAudioSubGroup             = "audioSubGroup";
static const std::string kParamAudioFilePath             = "audioFilePath";
static const std::string kParamAudioStreamId             = "audioStreamId";
static const std::string kParamAudioPreset               = "audioPreset";

static const size_t maxNbAudioStream                     = 16;

static const std::string kParamMetaAlbum           = "meta_album";
static const std::string kParamMetaAlbumArtist     = "meta_album_artist";
static const std::string kParamMetaArtist          = "meta_artist";
static const std::string kParamMetaComment         = "meta_comment";
static const std::string kParamMetaComposer        = "meta_composer";
static const std::string kParamMetaCopyright       = "meta_copyright";
static const std::string kParamMetaCreationTime    = "meta_creation_time";
static const std::string kParamMetaDate            = "meta_date";
static const std::string kParamMetaDisc            = "meta_disc";
static const std::string kParamMetaEncoder         = "meta_encoder";
static const std::string kParamMetaEncodedBy       = "meta_encoded_by";
static const std::string kParamMetaFilename        = "meta_filename";
static const std::string kParamMetaGenre           = "meta_genre";
static const std::string kParamMetaLanguage        = "meta_language";
static const std::string kParamMetaPerformer       = "meta_performer";
static const std::string kParamMetaPublisher       = "meta_publisher";
static const std::string kParamMetaServiceName     = "meta_service_name";
static const std::string kParamMetaServiceProvider = "meta_service_provider";
static const std::string kParamMetaTitle           = "meta_title";
static const std::string kParamMetaTrack           = "meta_track";
static const std::string kParamMetaVariantBitrate  = "meta_variant_bitrate";

}
}
}
}

#endif
