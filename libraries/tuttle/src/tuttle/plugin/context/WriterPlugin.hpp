#ifndef _TUTTLE_PLUGIN_CONTEXT_WRITERPLUGIN_HPP_
#define	_TUTTLE_PLUGIN_CONTEXT_WRITERPLUGIN_HPP_

#include "WriterDefinition.hpp"
#include <tuttle/plugin/FilenameManager.hpp>

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
	OFX::PushButtonParam* _renderButton;     ///< Render push button
	OFX::StringParam*     _filepath;         ///< Target file path
	OFX::BooleanParam*    _renderAlways;     ///< Render always
	OFX::ChoiceParam*     _bitDepth;         ///< Bit depth
	FilenameManager       _filePattern;         ///< Filename pattern manager

// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _srcClip;       ///< Input image clip
	OFX::Clip* _dstClip;       ///< Ouput image clip

};

inline bool WriterPlugin::varyOnTime() const
{
	///@todo tuttle: do this in FilenameManager
	return _filePattern.getFirstFilename() != _filepath->getValue();
}

}
}

#endif	/* _WRITERPLUGIN_HPP */

