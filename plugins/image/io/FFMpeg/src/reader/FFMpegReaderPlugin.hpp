#ifndef _TUTTLE_PLUGIN_FFMPEG_READER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_READER_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <ffmpeg/VideoFFmpegReader.hpp>

#include <string>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace reader {

struct FFMpegReaderParams
{
	std::string _filepath;      ///< Ffmpeg filepath
};

/**
 * @brief FFMpeg plugin
 */
class FFMpegReaderPlugin : public OFX::ImageEffect
{
public:
	FFMpegReaderPlugin( OfxImageEffectHandle handle );

public:
	bool ensureVideoIsOpen();

	FFMpegReaderParams getProcessParams() const;

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	bool getTimeDomain( OfxRangeD& range );

	void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
	void render( const OFX::RenderArguments& args );
	void endSequenceRender( const OFX::EndSequenceRenderArguments& args );

public:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _clipDst;              ///< Destination image clip

	OFX::StringParam* _paramFilepath; ///< video filepath

	bool _errorInFile;
	VideoFFmpegReader _reader;
};

}
}
}
}

#endif
