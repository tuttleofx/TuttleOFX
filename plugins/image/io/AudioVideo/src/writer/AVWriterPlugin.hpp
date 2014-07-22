#ifndef _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_AV_WRITER_PLUGIN_HPP_

#include <libav/LibAVPresetDefinitions.hpp>
#include <libav/LibAVVideoWriter.hpp>
#include <libav/LibAVOptions.hpp>

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
	int         _videoCodec;  ///< Video codec
	int         _audioCodec;  ///< Audio codec
	
	int         _videoPreset; ///< video configuration (based on the video codec)
	int         _audioPreset; ///< video configuration (based on the video codec)
	
	PixelFormat _videoPixelFormat; /// videoPixelFormat

	std::map< std::string, std::string > _metadatas;
};

/**
 * @brief LibAV plugin
 */
class AVWriterPlugin : public AVOptionPlugin< WriterPlugin >
{
public:
	AVWriterPlugin( OfxImageEffectHandle handle );

private:
	void updateVisibleTools();

public:
	AVProcessParams getProcessParams();

	void disableAVOptionsForCodecOrFormat( const std::vector<AVPrivOption>& avPrivOpts, const std::string& codec );
	void updatePixelFormat( const std::string& videoCodecName );

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
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

	LibAVVideoWriter    _writer;
	bool                _initWriter;
};

}
}
}
}

#endif
