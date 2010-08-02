#ifndef _TUTTLE_PLUGIN_CONTEXT_WRITERPLUGIN_HPP_
#define	_TUTTLE_PLUGIN_CONTEXT_WRITERPLUGIN_HPP_

#include "WriterDefinition.hpp"

#include <tuttle/common/clip/Sequence.hpp>

#include <ofxsImageEffect.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {

class WriterPlugin : public OFX::ImageEffect {
public:
	WriterPlugin( OfxImageEffectHandle handle );
	virtual ~WriterPlugin();

public:
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

protected:
	inline bool varyOnTime() const;

protected:
	common::Sequence       _filePattern;         ///< Filename pattern manager

	/// @group Attributes
	/// @{
	OFX::PushButtonParam* _paramRenderButton;     ///< Render push button
	OFX::StringParam*     _paramFilepath;         ///< Target file path
	OFX::BooleanParam*    _paramRenderAlways;     ///< Render always
	OFX::ChoiceParam*     _paramBitDepth;         ///< Bit depth

	OFX::Clip* _clipSrc;       ///< Input image clip
	OFX::Clip* _clipDst;       ///< Ouput image clip
	/// @}
};

inline bool WriterPlugin::varyOnTime() const
{
	///@todo tuttle: do this in FilenameManager
	return _filePattern.getFirstFilename() != _paramFilepath->getValue();
}

}
}

#endif

