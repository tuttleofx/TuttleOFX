/**
 * @file EXRWriterPlugin.hpp
 * @brief
 * @author
 * @date    16/12/09 15:34
 *
 */

#ifndef EXRWRITER_PLUGIN_H
#define EXRWRITER_PLUGIN_H

#include "EXRWriterDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <boost/gil/gil_all.hpp>
#include <tuttle/plugin/FilenameManager.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

struct EXRWriterParams
{
	std::string _filepath;       ///< filepath
	EBitDepth   _precision;		 ///< Bit depth
	ECompType   _componentsType; ///< Components type
};

/**
 * @brief
 *
 */
class EXRWriterPlugin : public OFX::ImageEffect
{
public:
	EXRWriterPlugin( OfxImageEffectHandle handle );
	OFX::Clip* getSrcClip() const;
	OFX::Clip* getDstClip() const;
	EXRWriterParams getParams(const OfxTime time);

public:
	virtual void render( const OFX::RenderArguments& args );
	void         changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

protected:
	OFX::PushButtonParam* _renderButton;    ///< Render push button
	OFX::StringParam*     _filepath;        ///< Target file path
	OFX::ChoiceParam*     _bitDepth;        ///< Bit depth
	OFX::ChoiceParam*     _componentsType;  ///< Components type
	OFX::BooleanParam*    _renderAlways;    ///< Render always
	FilenameManager       _fPattern;        ///< Filename pattern manager

	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _srcClip;       ///< Source image clip
	OFX::Clip* _dstClip;       ///< Ouput image clip
};

}
}
}
}

#endif  // EXRWRITER_PLUGIN_H
