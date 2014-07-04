#ifndef _TUTTLE_PLUGIN_AV_READER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_AV_READER_PLUGIN_HPP_

#include <AvTranscoder/File/InputFile.hpp>
#include <AvTranscoder/EssenceStream/InputVideo.hpp>
#include <AvTranscoder/DatasStructures/Image.hpp>
#include <AvTranscoder/EssenceTransform/VideoEssenceTransform.hpp>

#include <tuttle/plugin/context/ReaderPlugin.hpp>

#include <boost/scoped_ptr.hpp>

#include <string>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

struct AVReaderParams
{
	std::string _filepath;      ///< Ffmpeg filepath
};

/**
 * @brief AudioVideo plugin
 */
class AVReaderPlugin : public ReaderPlugin
{
public:
	AVReaderPlugin( OfxImageEffectHandle handle );

public:
	void ensureVideoIsOpen();
	
	AVReaderParams getProcessParams() const;

	void updateVisibleTools();
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	bool getTimeDomain( OfxRangeD& range );

	void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
	void render( const OFX::RenderArguments& args );

	inline bool varyOnTime() const { return true; }

private:
	double retrievePAR();
	
public:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip*         _clipDst;           ///< Destination image clip

	OFX::IntParam* _paramVideoStreamIndex; ///< video stream index
	OFX::BooleanParam* _paramUseCustomSAR; ///< Keep sample aspect ratio
	OFX::DoubleParam*  _paramCustomSAR;    ///< Custom SAR to use
	
	boost::scoped_ptr<avtranscoder::InputFile> _inputFile;
	boost::scoped_ptr<avtranscoder::InputVideo> _inputStreamVideo;
	boost::scoped_ptr<avtranscoder::Image> _sourceImage;
	boost::scoped_ptr<avtranscoder::Image> _imageToDecode;
	
	avtranscoder::VideoEssenceTransform _colorTransform;
	
	std::string _lastInputFilePath;
	size_t _idVideoStream;
	int _lastFrame;
};

}
}
}
}

#endif
