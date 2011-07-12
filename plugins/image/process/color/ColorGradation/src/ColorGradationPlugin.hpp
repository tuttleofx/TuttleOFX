#ifndef _TUTTLE_PLUGIN_COLORGRADATION_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORGRADATION_PLUGIN_HPP_

#include "ColorGradationDefinitions.hpp"
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace colorGradation {

template<typename Scalar>
struct ColorGradationProcessParams
{
	EParamGradation _in;
	EParamGradation _out;
	bool _processAlpha;
};

/**
 * @brief ColorGradation plugin
 */
class ColorGradationPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;

public:
	ColorGradationPlugin( OfxImageEffectHandle handle );

public:
	ColorGradationProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	void render( const OFX::RenderArguments& args );

public:
	OFX::ChoiceParam* _paramIn;
	OFX::ChoiceParam* _paramOut;
	OFX::BooleanParam* _paramProcessAlpha;
};

}
}
}

#endif
