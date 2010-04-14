#ifndef _PNG_WRITER_PLUGIN_HPP_
#define _PNG_WRITER_PLUGIN_HPP_

#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>
#include <tuttle/plugin/FilenameManager.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace writer {

using namespace boost::gil;

struct PNGWriterParams
{
	std::string _filepath;      ///< filepath
	int _precision;				///< Precision
};

/**
 * @brief Png writer
 */
class PNGWriterPlugin : public OFX::ImageEffect
{
public:
	PNGWriterPlugin( OfxImageEffectHandle handle );
	OFX::Clip* getSrcClip() const;
	OFX::Clip* getDstClip() const;
	PNGWriterParams getParams(const OfxTime time);

public:
	virtual void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );
	void getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences );
protected:
	OFX::PushButtonParam* _renderButton;  ///< Render push button
	OFX::StringParam*  _filepath;         ///< Target file path
	OFX::BooleanParam* _renderAlways;     ///< Render always
	OFX::ChoiceParam    *_precision;
	OFX::ChoiceParam    *_precisionLong;
	FilenameManager    _fPattern;         ///< Filename pattern manager

// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _srcClip;       ///< Input image clip
	OFX::Clip* _dstClip;       ///< Ouput image clip
};

}
}
}
}

#endif
