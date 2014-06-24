#ifndef _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_

#include <AvTranscoder/InputFile.hpp>
#include <AvTranscoder/OutputFile.hpp>
#include <AvTranscoder/OutputStreamVideo.hpp>
#include <AvTranscoder/InputStreamAudio.hpp>
#include <AvTranscoder/OutputStreamAudio.hpp>
#include <AvTranscoder/DummyInputStream.hpp>
#include <AvTranscoder/OptionLoader.hpp>
#include <AvTranscoder/ColorTransform.hpp>
#include <AvTranscoder/AudioTransform.hpp>

#include <AvTranscoder/DatasStructures/Image.hpp>

#include <tuttle/plugin/context/WriterPlugin.hpp>

#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <string>
#include <vector>

namespace tuttle {
namespace plugin {
namespace av {
namespace writer {

struct AVProcessParams
{
	std::string _outputFilePath; ///< Filepath
	std::vector<std::string> _inputAudioFilePath; ///< Filepath of audio input
	
	int         _format;      ///< Format
	std::string _formatName;      ///< Format name
	
	int         _videoCodec;  ///< Video codec
	std::string _videoCodecName;  ///< Video codec name
	
	int         _audioCodec;  ///< Audio codec
	std::string _audioCodecName;  ///< Audio codec name
	
	int         _videoPreset; ///< video configuration (based on the video codec)
	int         _audioPreset; ///< video configuration (based on the video codec)
	
	AVPixelFormat _videoPixelFormat; /// videoPixelFormat

	std::map< std::string, std::string > _metadatas;
};

/**
 * @brief LibAV plugin
 */
class AVWriterPlugin : public WriterPlugin
{
public:
	AVWriterPlugin( OfxImageEffectHandle handle );

private:
	void updateVisibleTools();

public:
	AVProcessParams getProcessParams();
	
	void disableAVOptionsForCodecOrFormat( avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& codec );
	void updatePixelFormat( const std::string& videoCodecName );
	void updateAudioStreamParams();

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

	void ensureVideoIsInit( const OFX::RenderArguments& args, AVProcessParams& params );
	void ensureAudioIsInit( AVProcessParams& params );

	/**
	 * @brief Called before each new render.
     */
	void cleanVideoAndAudio();
	
	void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
	void render( const OFX::RenderArguments& args );
	void endSequenceRender( const OFX::EndSequenceRenderArguments& args );
	
public:
	OFX::ChoiceParam*   _paramFormat;
	OFX::ChoiceParam*   _paramVideoCodec;
	OFX::ChoiceParam*   _paramAudioCodec;
	
	OFX::ChoiceParam*   _paramMainPreset;
	OFX::ChoiceParam*   _paramFormatPreset;
	OFX::ChoiceParam*   _paramMainVideoPreset;
	OFX::ChoiceParam*   _paramMainAudioPreset;
	
	OFX::IntParam*      _paramBitRate;
	OFX::BooleanParam*  _paramUseCustomFps;
	OFX::DoubleParam*   _paramCustomFps;
	OFX::ChoiceParam*   _paramVideoPixelFormat;
	
	OFX::GroupParam*    _paramAudioCustomGroup;
	
	OFX::IntParam*      _paramAudioNbStream;
	
	std::vector<OFX::GroupParam*> _paramAudioSubGroup;
	std::vector<OFX::StringParam*> _paramAudioFilePath;
	std::vector<OFX::IntParam*> _paramAudioStreamIndex;
	std::vector<OFX::ChoiceParam*> _paramAudioPreset;
	
	std::vector<OFX::StringParam*> _paramMetadatas;
	
	// to manage video
	boost::scoped_ptr<avtranscoder::OutputFile> _outputFile;
	avtranscoder::OutputStreamVideo _outputStreamVideo;
	
	// to manage audio
	boost::ptr_vector<avtranscoder::InputFile> _inputAudioFile;
	boost::ptr_vector<avtranscoder::InputStreamAudio> _inputStreamAudio;
	std::vector<avtranscoder::DummyInputStream> _dummyStreamAudio; // For silent audio tracks
	std::vector<avtranscoder::OutputStreamAudio> _outputStreamAudio; // For audio from input files
	std::vector<avtranscoder::OutputStreamAudio> _outputStreamAudioSilent; // For silent audio tracks
	
	// to process video encode
	boost::scoped_ptr<avtranscoder::Image> _rgbImage; // Between gil and avTranscoder convert
	boost::scoped_ptr<avtranscoder::Image> _imageToEncode; // Between avTranscoder convert and avTranscoder encode
	avtranscoder::ColorTransform _colorTransform;
	
	// to process audio encode
	avtranscoder::AudioTransform _audioTransform;
	
	// to manage OFX params depending on AVOptions
	avtranscoder::OptionLoader _optionLoader;
	
	std::vector<size_t> _audioStreamId;
		
	bool _initVideo;
	bool _initAudio;
	bool _initOutpuFile;
};

}
}
}
}

#endif
