#ifndef _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_

#include <AvTranscoder/InputFile.hpp>
#include <AvTranscoder/OutputFile.hpp>
#include <AvTranscoder/OutputStreamVideo.hpp>
#include <AvTranscoder/InputStreamAudio.hpp>
#include <AvTranscoder/OutputStreamAudio.hpp>
#include <AvTranscoder/OptionLoader.hpp>
#include <AvTranscoder/ColorTransform.hpp>
#include <AvTranscoder/AudioTransform.hpp>

#include <AvTranscoder/DatasStructures/Image.hpp>

#include <tuttle/plugin/context/WriterPlugin.hpp>

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
	std::string _inputAudioFilePath; ///< Filepath of audio input
	
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

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	
	void ensureWriterIsInit( const OFX::RenderArguments& args, AVProcessParams& params );
	void ensureAudioIsInit( AVProcessParams& params );
	
	void render( const OFX::RenderArguments& args );
	void endSequenceRender( const OFX::EndSequenceRenderArguments& args );
	
public:
	OFX::ChoiceParam*   _paramFormat;
	OFX::ChoiceParam*   _paramVideoCodec;
	OFX::ChoiceParam*   _paramAudioCodec;
	
	OFX::ChoiceParam*   _paramMainPreset;
	OFX::ChoiceParam*   _paramFormatPreset;
	OFX::ChoiceParam*   _paramVideoCodecPreset;
	OFX::ChoiceParam*   _paramAudioCodecPreset;
	
	OFX::IntParam*      _paramBitRate;
	OFX::BooleanParam*  _paramUseCustomFps;
	OFX::DoubleParam*   _paramCustomFps;
	OFX::ChoiceParam*   _paramVideoPixelFormat;
	
	OFX::IntParam*      _paramAudioNbStream;
	OFX::StringParam*   _paramAudioFilePath;
	OFX::IntParam*      _paramAudioStreamIndex;
	
	std::vector<OFX::StringParam*> _paramMetadatas;
	
	boost::scoped_ptr<avtranscoder::OutputFile> _outputFile;
	boost::scoped_ptr<avtranscoder::OutputStreamVideo> _outputStreamVideo;
	
	boost::scoped_ptr<avtranscoder::InputFile> _inputAudioFile;
	boost::scoped_ptr<avtranscoder::InputStreamAudio> _inputStreamAudio;
	boost::scoped_ptr<avtranscoder::OutputStreamAudio> _outputStreamAudio;
	
	avtranscoder::OptionLoader _optionLoader;
	
	// to process video encode
	avtranscoder::ColorTransform _colorTransform;
	boost::scoped_ptr<avtranscoder::Image> _rgbImage; // Between gil and avTranscoder convert
	boost::scoped_ptr<avtranscoder::Image> _imageToEncode; // Between avTranscoder convert and avTranscoder encode
	// to process audio encode
	avtranscoder::AudioTransform _audioTransform;
	
	std::string _lastInputAudioFilePath;
	std::string _lastOutputFilePath;
	
	size_t _idAudioStream;
	
	bool _initWriter;
	bool _initAudio;
};

}
}
}
}

#endif
