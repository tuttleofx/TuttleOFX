#ifndef _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_

#include <common/util.hpp>

#include <tuttle/plugin/context/WriterPlugin.hpp>

#include <AvTranscoder/util.hpp>
#include <AvTranscoder/Profile.hpp>
#include <AvTranscoder/transcoder/Transcoder.hpp>
#include <AvTranscoder/file/OutputFile.hpp>
#include <AvTranscoder/codec/VideoCodec.hpp>
#include <AvTranscoder/frame/Frame.hpp>

#include <boost/scoped_ptr.hpp>

#include <string>
#include <vector>

namespace tuttle {
namespace plugin {
namespace av {
namespace writer {

struct AVProcessParams
{
	std::string _outputFilePath; ///< Filepath
	
	int         _format;      ///< Format
	std::string _formatName;      ///< Format name
	
	int         _videoCodec;  ///< Video codec
	std::string _videoCodecName;  ///< Video codec name
	
	int         _audioCodec;  ///< Audio codec
	std::string _audioCodecName;  ///< Audio codec name
	
	std::string _videoPixelFormatName; /// videoPixelFormat name
	std::string _audioSampleFormatName; /// audioSampleFormat name

	avtranscoder::MetadatasMap _metadatas;
};

/**
 * @brief AudioVideo plugin
 */
class AVWriterPlugin : public WriterPlugin
{
public:
	AVWriterPlugin( OfxImageEffectHandle handle );

private:
	void updateVisibleTools();

public:
	AVProcessParams getProcessParams();
	
	void updateSampleFormats( const std::string& audioCodecName );
	void updatePixelFormats( const std::string& videoCodecName );
	void updateAudioParams();
	void updateAudioSilent( size_t indexAudioOutput );
	void updateAudioSelectStream( size_t indexAudioOutput );
	void updateAudioRewrap( size_t indexAudioOutput );
	void updateAudioFileInfo( size_t indexAudioOutput );
	
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

	void initOutput();
	void initAudio();
	void ensureVideoIsInit( const OFX::RenderArguments& args );

	bool isOutputInit();
	
	void updateFormatFromExistingProfile();
	void updateVideoFromExistingProfile();
	void updateAudiotFromExistingProfile();

	/**
	 * @brief Called before each new render.
     */
	void cleanVideoAndAudio();
	
	void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
	void render( const OFX::RenderArguments& args );
	void endSequenceRender( const OFX::EndSequenceRenderArguments& args );
	
public:
	// format
	OFX::GroupParam* _paramFormatCustomGroup;
	OFX::ChoiceParam* _paramFormat;
	
	// video
	OFX::GroupParam* _paramVideoCustomGroup;
	OFX::ChoiceParam* _paramVideoCodec;
	
	// audio
	OFX::GroupParam* _paramAudioCustomGroup;
	OFX::ChoiceParam* _paramAudioCodec;
	
	OFX::IntParam* _paramAudioNbInput;
	std::vector<OFX::GroupParam*> _paramAudioSubGroup;
	std::vector<OFX::BooleanParam*> _paramAudioSilent;
	std::vector<OFX::StringParam*> _paramAudioFilePath;
	std::vector<OFX::StringParam*> _paramAudioFileInfo;
	std::vector<OFX::BooleanParam*> _paramAudioSelectStream;
	std::vector<OFX::IntParam*> _paramAudioStreamIndex;
	std::vector<OFX::ChoiceParam*> _paramAudioPreset;
	std::vector<OFX::IntParam*> _paramAudioOffset;
	
	// custom params
	OFX::BooleanParam* _paramUseCustomFps;
	OFX::DoubleParam* _paramCustomFps;
	OFX::ChoiceParam* _paramVideoPixelFormat;
	OFX::ChoiceParam* _paramAudioSampleFormat;
	
	common::LibAVParams _paramFormatCustom;
	common::LibAVParams _paramVideoCustom;
	common::LibAVParams _paramAudioCustom;
	
	common::LibAVParams _paramFormatDetailCustom;
	common::LibAVParams _paramVideoDetailCustom;
	common::LibAVParams _paramAudioDetailCustom;
	
	// presets
	OFX::ChoiceParam* _paramMainPreset;
	OFX::ChoiceParam* _paramFormatPreset;
	OFX::ChoiceParam* _paramVideoPreset;
	OFX::ChoiceParam* _paramAudioMainPreset;
	
	// metadata
	std::vector<OFX::StringParam*> _paramMetadatas;
	
	// to process transcode
	boost::scoped_ptr<avtranscoder::OutputFile> _outputFile;
	boost::scoped_ptr<avtranscoder::Transcoder> _transcoder;
	
	// to process video
	avtranscoder::Frame _videoFrame;
	avtranscoder::VideoCodec _videoCodec;

	avtranscoder::Profile _presets;

	std::string _lastOutputFilePath;  ///< To check if output file path has changed.

	double _outputFps;

	bool _initVideo;  ///< To check if video stream is init.
	bool _initWrap;  ///< To check if initial wrap of output file is done.
};

}
}
}
}

#endif
