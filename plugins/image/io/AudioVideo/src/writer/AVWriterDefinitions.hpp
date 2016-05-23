#ifndef _TUTTLE_PLUGIN_AV_WRITER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_AV_WRITER_DEFINITIONS_HPP_

#include <tuttle/ioplugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>

#include <common/LibAVParams.hpp>

namespace tuttle
{
namespace plugin
{
namespace av
{
namespace writer
{

enum ETuttlePluginBitDepth
{
    eTuttlePluginBitDepth8 = 0
};

static const std::string kParamFormat = common::kPrefixFormat + "format";
static const std::string kParamFormatLabel = "Format";
static const std::string kParamVideoCodec = common::kPrefixVideo + "codec";
static const std::string kParamVideoCodecLabel = "Video codec";
static const std::string kParamAudioCodec = common::kPrefixAudio + "codec";
static const std::string kParamAudioCodecLabel = "Audio codec";

static const std::string kParamFormatGroup = common::kPrefixFormat + "group";
static const std::string kParamFormatDetailedGroup = common::kPrefixFormat + "detailedGroup";
static const std::string kParamVideoGroup = common::kPrefixVideo + "group";
static const std::string kParamVideoDetailedGroup = common::kPrefixVideo + "detailedGroup";
static const std::string kParamAudioGroup = common::kPrefixAudio + "group";
static const std::string kParamAudioDetailedGroup = common::kPrefixAudio + "detailedGroup";
static const std::string kParamMetaGroup = common::kPrefixMetaData + "group";
static const std::string kParamAboutGroup = common::kPrefixAbout + "group";

static const std::string kParamMainPreset = "mainPreset";

static const std::string kParamFormatPreset = common::kPrefixFormat + "preset";
static const std::string kParamVideoPreset = common::kPrefixVideo + "preset";
static const std::string kParamAudioMainPreset = common::kPrefixAudio + "mainPreset";

static const std::string kParamUseCustomFps = common::kPrefixVideo + "useCustomFps";
static const std::string kParamCustomFps = common::kPrefixVideo + "customFps";

static const std::string kParamUseCustomSize = common::kPrefixVideo + "useCustomSize";
static const std::string kParamCustomSize = common::kPrefixVideo + "customSize";

static const std::string kParamVideoCodecPixelFmt = common::kPrefixVideo + "pixelFormat";
static const std::string kParamAudioCodecSampleFmt = common::kPrefixAudio + "sampleFormat";

static const std::string kParamFormatCustomGroup = common::kPrefixFormat + "customGroup";
static const std::string kParamVideoCustomGroup = common::kPrefixVideo + "customGroup";
static const std::string kParamAudioCustomGroup = common::kPrefixAudio + "customGroup";

static const std::string kParamAudioNbInputs = common::kPrefixAudio + "nbInput";

static const std::string kParamAudioSubGroup = common::kPrefixAudio + "subGroup";
static const std::string kParamAudioSilent = common::kPrefixAudio + "silent";
static const std::string kParamAudioFilePath = common::kPrefixAudio + "filePath";
static const std::string kParamAudioFileInfo = common::kPrefixAudio + "fileInfo";
static const std::string kParamAudioSelectStream = common::kPrefixAudio + "selectStream";
static const std::string kParamAudioStreamIndex = common::kPrefixAudio + "streamIndex";
static const std::string kParamAudioPreset = common::kPrefixAudio + "preset";
static const std::string kParamAudioOffset = common::kPrefixAudio + "offset";

static const size_t maxNbAudioInput = 16;

static const std::string kParamMetaAlbum = common::kPrefixMetaData + "album";
static const std::string kParamMetaAlbumArtist = common::kPrefixMetaData + "album_artist";
static const std::string kParamMetaArtist = common::kPrefixMetaData + "artist";
static const std::string kParamMetaComment = common::kPrefixMetaData + "comment";
static const std::string kParamMetaComposer = common::kPrefixMetaData + "composer";
static const std::string kParamMetaCopyright = common::kPrefixMetaData + "copyright";
static const std::string kParamMetaCreationTime = common::kPrefixMetaData + "creation_time";
static const std::string kParamMetaDate = common::kPrefixMetaData + "date";
static const std::string kParamMetaDisc = common::kPrefixMetaData + "disc";
static const std::string kParamMetaEncodedBy = common::kPrefixMetaData + "encoded_by";
static const std::string kParamMetaFilename = common::kPrefixMetaData + "filename";
static const std::string kParamMetaGenre = common::kPrefixMetaData + "genre";
static const std::string kParamMetaLanguage = common::kPrefixMetaData + "language";
static const std::string kParamMetaPerformer = common::kPrefixMetaData + "performer";
static const std::string kParamMetaPublisher = common::kPrefixMetaData + "publisher";
static const std::string kParamMetaServiceName = common::kPrefixMetaData + "service_name";
static const std::string kParamMetaServiceProvider = common::kPrefixMetaData + "service_provider";
static const std::string kParamMetaTitle = common::kPrefixMetaData + "title";
static const std::string kParamMetaTrack = common::kPrefixMetaData + "track";
static const std::string kParamMetaVariantBitrate = common::kPrefixMetaData + "variant_bitrate";

static const std::string kParamAboutLibName = common::kPrefixAbout + "name";
static const std::string kParamAboutLicense = common::kPrefixAbout + "license";
static const std::string kParamAboutLicenseLabel = "License";
static const std::string kParamAboutVersion = common::kPrefixAbout + "version";
static const std::string kParamAboutVersionLabel = "Version";

static const std::string kParamVerbose = "verbose";
}
}
}
}

#endif
