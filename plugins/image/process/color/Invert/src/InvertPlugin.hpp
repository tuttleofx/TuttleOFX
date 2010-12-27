#ifndef _TUTTLE_PLUGIN_INVERTPLUGIN_HPP_
#define _TUTTLE_PLUGIN_INVERTPLUGIN_HPP_

#include "InvertDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

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
class InvertPlugin : public ImageEffectGilPlugin
{
public:
	InvertPlugin( OfxImageEffectHandle handle );
	
public:
	InvertProcessParams getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

 	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	void render( const OFX::RenderArguments& args );

protected:
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
