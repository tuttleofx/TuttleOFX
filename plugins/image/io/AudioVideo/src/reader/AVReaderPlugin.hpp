#ifndef _TUTTLE_PLUGIN_AV_READER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_AV_READER_PLUGIN_HPP_

#include <common/util.hpp>

#include <tuttle/plugin/context/ReaderPlugin.hpp>

#include <AvTranscoder/util.hpp>
#include <AvTranscoder/file/InputFile.hpp>
#include <AvTranscoder/decoder/VideoDecoder.hpp>
#include <AvTranscoder/frame/VideoFrame.hpp>
#include <AvTranscoder/transform/VideoTransform.hpp>

#include <boost/scoped_ptr.hpp>

#include <string>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

struct AVReaderParams
{
	std::string _filepath;
	std::string _inputFormatName;

	const avtranscoder::FileProperties* _inputProperties;
	const avtranscoder::VideoProperties* _inputVideoProperties;
};

/**
 * @brief AudioVideo plugin
 */
class AVReaderPlugin : public ReaderPlugin
{
public:
	AVReaderPlugin( OfxImageEffectHandle handle );

public:
	/**
	 * @brief Open and analyse the InputFile. 
	 * Create a Stream and its corresponding Decoder for the first video stream of the given file.
	 * @note Do nothing if the InputFile and the videoStream are already set
	 */
	void ensureVideoIsOpen();

	/**
	 * @brief Clear all attributes related to unwrapping / decoding.
	 */
	void cleanInputFile();

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
	/**
	* @brief Retrieve Pixel Aspect Ratio from parameters (if custom) or from video file.
	* @warning video have to be open (see ensureVideoIsOpen)
	*/
	double retrievePAR();
	
public:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip*         _clipDst;           ///< Destination image clip

	OFX::IntParam* _paramVideoStreamIndex; ///< video stream index
	OFX::BooleanParam* _paramUseCustomSAR; ///< Keep sample aspect ratio
	OFX::DoubleParam*  _paramCustomSAR;    ///< Custom SAR to use
	
	common::LibAVParams _paramFormatCustom;
	common::LibAVParams _paramVideoCustom;
	common::LibAVParams _paramMetaDataCustom;
	
	common::LibAVParams _paramFormatDetailCustom;
	common::LibAVParams _paramVideoDetailCustom;
	
	OFX::StringParam* _paramMetaDataWrapper;
	OFX::StringParam* _paramMetaDataVideo;
	OFX::StringParam* _paramMetaDataAudio;
	OFX::StringParam* _paramMetaDataData;
	OFX::StringParam* _paramMetaDataSubtitle;
	OFX::StringParam* _paramMetaDataAttachement;
	OFX::StringParam* _paramMetaDataUnknown;

	OFX::BooleanParam* _paramVerbose;
	
	boost::scoped_ptr<avtranscoder::InputFile> _inputFile;
	boost::scoped_ptr<avtranscoder::VideoDecoder> _inputDecoder;
	boost::scoped_ptr<avtranscoder::VideoFrame> _sourceImage;
	boost::scoped_ptr<avtranscoder::VideoFrame> _imageToDecode;
	avtranscoder::InputStream* _inputStream;  ///< Has link (InputFile has ownership)
	
	avtranscoder::VideoTransform _colorTransform;
	
	std::string _lastInputFilePath;
	size_t _lastVideoStreamIndex;
	
	int _lastFrame;
	
	bool _initVideo;
};

}
}
}
}

#endif
