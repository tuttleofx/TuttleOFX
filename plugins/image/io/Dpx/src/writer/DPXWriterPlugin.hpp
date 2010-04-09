#ifndef DPXWRITER_PLUGIN_HPP
#define DPXWRITER_PLUGIN_HPP

#include "../dpxEngine/dpxImage.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>
#include <tuttle/plugin/FilenameManager.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

struct DPXWriterParams
{
	std::string _filepath;      ///< filepath
	int _bitDepth;				///< Bit depth
	int _componentsType;		///< Components type
	bool _compressed;			///< Bit streaming
};

/**
 * @brief
 *
 */
class DPXWriterPlugin : public OFX::ImageEffect
{
public:
	DPXWriterPlugin( OfxImageEffectHandle handle );
	OFX::Clip* getSrcClip() const;
	OFX::Clip* getDstClip() const;
	DPXWriterParams getParams(const OfxTime time);

public:
	virtual void render( const OFX::RenderArguments& args );
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

protected:
	OFX::PushButtonParam* _renderButton;  ///< Render push button
	OFX::StringParam*	       _filepath;        ///< Dpx file path
	OFX::ChoiceParam*          _bitDepth;        ///< Dpx bit depth
	OFX::ChoiceParam*          _componentsType;  ///< Dpx components type
	OFX::BooleanParam*         _compressed;      ///< Dpx is bit streamed
	OFX::BooleanParam*         _renderAlways;    ///< Render always
	FilenameManager            _fPattern;        ///< Filename pattern manager
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _srcClip;       ///< Source image clip
	OFX::Clip* _dstClip;       ///< Ouput image clip

};

}
}
}
}

#endif
