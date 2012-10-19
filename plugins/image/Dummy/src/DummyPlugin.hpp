#ifndef _TUTTLE_PLUGIN_DUMMY_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_DUMMY_PLUGIN_HPP_

#include "DummyDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace dummy {

template<typename Scalar>
struct DummyProcessParams
{
	
};

/**
 * @brief Dummy plugin
 */
class DummyPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;
public:
	DummyPlugin( OfxImageEffectHandle handle );

public:
	DummyProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

	void changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName );

	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	void render( const OFX::RenderArguments &args );

};

}
}
}

#endif
