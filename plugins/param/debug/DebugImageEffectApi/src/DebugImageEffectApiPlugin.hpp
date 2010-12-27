#ifndef _TUTTLE_PLUGIN_DEBUGIMAGEEFFECTAPI_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_DEBUGIMAGEEFFECTAPI_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace debugImageEffectApi {

struct DebugImageEffectApiParams
{};

/**
 * @brief DebugImageEffectApi plugin
 */
class DebugImageEffectApiPlugin : public ImageEffectGilPlugin
{
public:
	DebugImageEffectApiPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	void      changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	DebugImageEffectApiParams getParams() const;

public:
	OFX::StringParam* _hostInfos;
	OFX::DoubleParam* _currentTime;
	OFX::DoubleParam* _beginTime;
	OFX::DoubleParam* _endTime;
};

}
}
}

#endif
