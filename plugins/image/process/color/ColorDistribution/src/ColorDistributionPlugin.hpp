#ifndef _TUTTLE_PLUGIN_COLORDISTRIBUTION_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORDISTRIBUTION_PLUGIN_HPP_

#include "ColorDistributionDefinitions.hpp"
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace colorDistribution {

template<typename Scalar>
struct ColorDistributionProcessParams
{
	EParamDistribution _in;
	EParamDistribution _out;
};

/**
 * @brief ColorDistribution plugin
 */
class ColorDistributionPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;

public:
	ColorDistributionPlugin( OfxImageEffectHandle handle );

public:
	ColorDistributionProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	bool isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime );

	void render( const OFX::RenderArguments& args );

public:
	OFX::BooleanParam* _paramInvert;
	OFX::ChoiceParam* _paramIn;
	OFX::ChoiceParam* _paramOut;
};

}
}
}

#endif
