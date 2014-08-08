#ifndef _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_

#include <common/util.hpp>

#include <tuttle/plugin/context/WriterPlugin.hpp>

#include <AvTranscoder/File/OutputFile.hpp>
#include <AvTranscoder/Transcoder/Transcoder.hpp>
#include <AvTranscoder/EssenceStream/DummyVideo.hpp>
#include <AvTranscoder/EssenceStructures/Frame.hpp>
#include <AvTranscoder/OptionLoader.hpp>
#include <AvTranscoder/Profile.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <string>
#include <vector>
#include <utility> // pair

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

	std::map< std::string, std::string > _metadatas;
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
	void updateAudioSilent();
	void updateAudioCopyStream();
	void updateAllChannels();
	
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

	void initOutput();
	void initAudio();
	void ensureVideoIsInit( const OFX::RenderArguments& args );

	bool isOutputInit();
	
	void updateFormatProfile();
	void updateVideoProfile();
	void updateAudioProfile();
	
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
	
	OFX::IntParam* _paramAudioNbStream;
	std::vector<OFX::GroupParam*> _paramAudioSubGroup;
	std::vector<OFX::BooleanParam*> _paramAudioSilent;
	std::vector<OFX::StringParam*> _paramAudioFilePath;
	std::vector<OFX::IntParam*> _paramAudioStreamIndex;
	std::vector<OFX::BooleanParam*> _paramAudioCopyStream;
	std::vector<OFX::ChoiceParam*> _paramAudioPreset;
	std::vector<OFX::BooleanParam*> _paramAudioAllChannels;
	std::vector<OFX::IntParam*> _paramAudioChannelIndex;
	
	// custom params
	OFX::BooleanParam* _paramUseCustomFps;
	OFX::DoubleParam* _paramCustomFps;
	OFX::ChoiceParam* _paramVideoPixelFormat;
	OFX::ChoiceParam* _paramAudioSampleFormat;
	
	common::CustomParams _paramFormatCustom;
	common::CustomParams _paramVideoCustom;
	common::CustomParams _paramAudioCustom;
	
	common::CustomParams _paramFormatDetailCustom;
	common::CustomParams _paramVideoDetailCustom;
	common::CustomParams _paramAudioDetailCustom;
	
	// presets
	OFX::ChoiceParam* _paramMainPreset;
	OFX::ChoiceParam* _paramFormatPreset;
	OFX::ChoiceParam* _paramVideoPreset;
	OFX::ChoiceParam* _paramMainAudioPreset;
	
	// metadata
	std::vector<OFX::StringParam*> _paramMetadatas;
	
	// to process transcode
	boost::scoped_ptr<avtranscoder::OutputFile> _outputFile;
	boost::scoped_ptr<avtranscoder::Transcoder> _transcoder;
	
	// to process video
	avtranscoder::Frame _videoFrame;
	avtranscoder::DummyVideo _dummyVideo;
	
	avtranscoder::OptionLoader _optionLoader;
	avtranscoder::Profile _presets;
	
	avtranscoder::Profile::ProfileDesc _formatProfile;
	avtranscoder::Profile::ProfileDesc _videoProfile;
	avtranscoder::Profile::ProfileDesc _audioProfile;
	
	std::string _lastOutputFilePath;
		
	bool _initVideo;
	bool _initWrap;
};

}
}
}
}

#endif
