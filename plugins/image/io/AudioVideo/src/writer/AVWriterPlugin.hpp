#ifndef _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_

#include <common/util.hpp>

#include <tuttle/ioplugin/context/WriterPlugin.hpp>

#include <AvTranscoder/transcoder/Transcoder.hpp>
#include <AvTranscoder/profile/ProfileLoader.hpp>
#include <AvTranscoder/file/OutputFile.hpp>
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
	
	int         _formatId;
	std::string _formatName;
	
	int         _videoCodecId;
	std::string _videoCodecName;
	
	int         _audioCodecId;
	std::string _audioCodecName;
	
	std::string _videoPixelFormatName;
	std::string _audioSampleFormatName;

	avtranscoder::PropertyVector _metadatas;
};

/**
 * @brief AudioVideo plugin
 */
class AVWriterPlugin : public WriterPlugin
{
public:
	AVWriterPlugin( OfxImageEffectHandle handle );

public:
	AVProcessParams getProcessParams();

	/**
	* @brief Update the list of pixel format supported depending on the video codec.
	* Warning: the function does not update the list correctly in Nuke.
	*/
	void updatePixelFormats( const std::string& videoCodecName );

	/**
	* @brief Update the list of sample format supported depending on the audio codec.
	* Warning: the function does not update the list correctly in Nuke.
	*/
	void updateSampleFormats( const std::string& audioCodecName );

	//@{
	/** Update display of OFX parameters releated to audio. */
	void updateAudioParams();
	void updateAudioSilent( size_t indexAudioOutput );
	void updateAudioSelectStream( size_t indexAudioOutput );
	void updateAudioOffset( size_t indexAudioOutput );
	void updateAudioFileInfo( size_t indexAudioOutput );
	//@}

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );

	void initOutput();  ///< Initialize output file and transcoder
	void initAudio();  ///< Initialize output audio streams
	void initVideo( const OFX::RenderArguments& args );  ///< Initialize output video stream

	bool isOutputInit();  ///< Check if output file and transcoder are initialized before render

	//@{
	/** Update OFX parameters from an existing profile (format, video, or audio). */
	void updateFormatFromExistingProfile();
	void updateVideoFromExistingProfile();
	void updateAudioFromExistingProfile();
	//@}

	void cleanVideoAndAudio();  ///< Called before each new render.

	void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
	void render( const OFX::RenderArguments& args );
	void endSequenceRender( const OFX::EndSequenceRenderArguments& args );

private:
	void updateVisibleTools();

	//@{
	/** Getters which throw exception if format or codec are not found. */
	std::string getFormatName( const size_t formatIndex ) const;
	std::string getVideoCodecName( const size_t codecIndex ) const;
	std::string getAudioCodecName( const size_t codecIndex ) const;
	std::string getPixelFormatName( const std::string& videoCodecName ) const;
	std::string getSampleFormatName( const std::string& audioCodecName ) const;
	//@}

	//@{
	void setFormatParam( const std::string& formatShortName );
	void setVideoCodecParam( const std::string& videoCodecShortName );
	void setAudioCodecParam( const std::string& audioCodecShortName );
	//@}

	/**
	 * @brief Before any render, the wrapper/encoder profiles are checked and possibly updated by this function:
	 * 1 - remove libav options which can make the wrapper/encoder failed if bad value
	 * 2 - add libav options which can't correspond to an OFX Choice parameter's value
	 * Example: An option can have a value only with a specific codec, but the corresponding
	 * OFX Choice doesn't know this value when instanciate the option in its factory.
	 */
	void cleanProfile( avtranscoder::ProfileLoader::Profile& profileToClean, const std::string& prefix );

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
	std::vector<OFX::DoubleParam*> _paramAudioOffset;
	
	// custom params
	OFX::BooleanParam* _paramUseCustomFps;
	OFX::DoubleParam* _paramCustomFps;

	OFX::BooleanParam* _paramUseCustomSize;
	OFX::Int2DParam* _paramCustomSize;  ///< width / height

	OFX::ChoiceParam* _paramVideoPixelFormat;
	OFX::ChoiceParam* _paramAudioSampleFormat;
	
	common::LibAVParams _paramFormatCustom;
	common::LibAVParams _paramVideoCustom;
	common::LibAVParams _paramAudioCustom;
	
	common::LibAVParams _paramFormatDetailCustom;
	common::LibAVParams _paramVideoDetailCustom;
	common::LibAVParams _paramAudioDetailCustom;

	OFX::BooleanParam* _paramVerbose;

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
	avtranscoder::ProfileLoader _presetLoader;

	bool _initVideo;  ///< To check if video stream is init.
	bool _initWrap;  ///< To check if initial wrap of output file is done.
};

}
}
}
}

#endif
