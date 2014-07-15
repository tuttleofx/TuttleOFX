#ifndef _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_

#include <common/util.hpp>

#include <tuttle/plugin/context/WriterPlugin.hpp>

#include <AvTranscoder/File/OutputFile.hpp>
#include <AvTranscoder/EssenceStream/OutputVideo.hpp>
#include <AvTranscoder/OptionLoader.hpp>
#include <AvTranscoder/Profile.hpp>
#include <AvTranscoder/EssenceTransform/VideoEssenceTransform.hpp>
#include <AvTranscoder/Transcoder/Transcoder.hpp>
#include <AvTranscoder/DatasStructures/Image.hpp>

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
	
	AVPixelFormat _videoPixelFormat; /// videoPixelFormat
	std::string _videoPixelFormatName; /// videoPixelFormat name

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
	
	void disableAVOptionsForCodecOrFormat( avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& codec, const std::string& prefix="" );
	void updatePixelFormat( const std::string& videoCodecName );
	void updateAudioParams();
	void updateAudioCopyStream();
	void updateAudioSilent();
	
	void fetchCustomParams( avtranscoder::OptionLoader::OptionMap& optionsMap, const std::string& prefix="" );

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

	void initOutput( AVProcessParams& params );
	void initAudio( AVProcessParams& params );
	void ensureVideoIsInit( const OFX::RenderArguments& args, AVProcessParams& params );

	/**
	 * @brief Called before each new render.
     */
	void cleanVideoAndAudio();
	
	void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
	void render( const OFX::RenderArguments& args );
	void endSequenceRender( const OFX::EndSequenceRenderArguments& args );
	
public:
	// format
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
	
	// custom params
	OFX::BooleanParam* _paramUseCustomFps;
	OFX::DoubleParam* _paramCustomFps;
	OFX::ChoiceParam* _paramVideoPixelFormat;
	
	common::CustomParams _paramVideoCustom;
	common::CustomParams _paramAudioCustom;
	
	// presets
	OFX::ChoiceParam* _paramMainPreset;
	OFX::ChoiceParam* _paramFormatPreset;
	OFX::ChoiceParam* _paramMainVideoPreset;
	OFX::ChoiceParam* _paramMainAudioPreset;
	
	// metadata
	std::vector<OFX::StringParam*> _paramMetadatas;
	
	boost::scoped_ptr<avtranscoder::OutputFile> _outputFile;
	avtranscoder::OutputVideo _outputStreamVideo;
	boost::scoped_ptr<avtranscoder::Transcoder> _transcoder;
	
	// to process video
	boost::scoped_ptr<avtranscoder::Image> _rgbImage; // Between gil and avTranscoder convert
	boost::scoped_ptr<avtranscoder::Image> _imageToEncode; // Between avTranscoder convert and avTranscoder encode
	avtranscoder::VideoEssenceTransform _colorTransform;
	
	avtranscoder::OptionLoader _optionLoader;
	avtranscoder::Profile _presets;
	
	std::string _lastOutputFilePath;
		
	bool _initVideo;
	bool _initWrap;
};

}
}
}
}

#endif
