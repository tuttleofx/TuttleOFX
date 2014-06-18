#ifndef _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_

#include <libav/LibAVPresetDefinitions.hpp>

#include <AvTranscoder/OutputFile.hpp>
#include <AvTranscoder/OutputStreamVideo.hpp>
#include <AvTranscoder/ColorTransform.hpp>
#include <AvTranscoder/OptionLoader.hpp>

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
	std::string _filepath;    ///< Filepath
	
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
	
	void ensureWriterIsInit( const OFX::RenderArguments& args );
	
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
	
	std::vector<OFX::StringParam*> _paramMetadatas;
	
	boost::scoped_ptr<avtranscoder::OutputFile> _outputFile;
	boost::scoped_ptr<avtranscoder::OutputStreamVideo> _outputStreamVideo;
	
	avtranscoder::ColorTransform _colorTransform;
	boost::scoped_ptr<avtranscoder::Image> _rgbImage; // between gil and avTranscoder convert
	boost::scoped_ptr<avtranscoder::Image> _imageToEncode; // betwwen avTranscoder convert and avTranscoder encode 
	
	avtranscoder::OptionLoader _optionLoader;
	
	bool _initWriter;
	
	std::string _lastOutputFilePath;
};

}
}
}
}

#endif
