#ifndef _TUTTLE_PLUGIN_AV_READER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_AV_READER_PLUGIN_HPP_

#include <tuttle/plugin/context/ReaderPlugin.hpp>

#include <libav/LibAVPresetDefinitions.hpp>
#include <libav/LibAVVideoReader.hpp>
#include <libav/LibAVOptions.hpp>

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
class AVReaderPlugin : public AVOptionPlugin< ReaderPlugin >
{
public:
	AVReaderPlugin( OfxImageEffectHandle handle );

public:
	bool ensureVideoIsOpen();

	AVReaderParams getProcessParams() const;

	void updateVisibleTools();
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	bool getTimeDomain( OfxRangeD& range );

	void beginSequenceRender( const OFX::BeginSequenceRenderArguments& args );
	void render( const OFX::RenderArguments& args );
	void endSequenceRender( const OFX::EndSequenceRenderArguments& args );

public:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip*         _clipDst;           ///< Destination image clip

	OFX::StringParam*  _paramFilepath;     ///< video filepath
	OFX::BooleanParam* _paramUseCustomSAR; ///< Keep sample aspect ratio
	OFX::DoubleParam*  _paramCustomSAR;    ///< Custom SAR to use

	bool               _errorInFile;
	bool               _initReader;
	LibAVVideoReader   _reader;
};

}
}
}
}

#endif
