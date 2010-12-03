#ifndef _TUTTLE_PLUGIN_INVERTPLUGIN_HPP_
#define _TUTTLE_PLUGIN_INVERTPLUGIN_HPP_

#include "InvertDefinitions.hpp"

#include <ofxsImageEffect.h>

namespace tuttle {
namespace plugin {
namespace invert {

struct InvertProcessParams
{
	bool _red;
	bool _green;
	bool _blue;
	bool _alpha;

};

/**
 * @brief
 *
 */
class InvertPlugin : public OFX::ImageEffect
{
public:
	InvertPlugin( OfxImageEffectHandle handle );
	
public:
	InvertProcessParams getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

protected:
	// do not need to delete these, the ImageEffect is managing them for us
	OFX::Clip* _clipSrc;      ///< Source image clip
	OFX::Clip* _clipDst;      ///< Destination image clip

	OFX::GroupParam* _processGroup;
	OFX::BooleanParam* _processR;
	OFX::BooleanParam* _processG;
	OFX::BooleanParam* _processB;
	OFX::BooleanParam* _processA;
};

}
}
}

#endif
