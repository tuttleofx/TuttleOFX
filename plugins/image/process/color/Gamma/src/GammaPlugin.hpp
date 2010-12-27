#ifndef _TUTTLE_PLUGIN_GAMMA_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_GAMMA_PLUGIN_HPP_

#include "GammaDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace gamma {

template<typename Scalar>
struct GammaProcessParams
{
	double iRGamma,
	       iGGamma,
	       iBGamma,
	       iAGamma;
};

/**
 * @brief Gamma plugin
 */
class GammaPlugin : public ImageEffectGilPlugin
{
public:
	typedef float Scalar;

public:
	GammaPlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	GammaProcessParams<Scalar> getProcessParams( const OfxPointD& renderScale = OFX::kNoRenderScale ) const;

public:
	OFX::ChoiceParam* _gammaType;
	OFX::DoubleParam* _master;
	OFX::DoubleParam* _red;
	OFX::DoubleParam* _green;
	OFX::DoubleParam* _blue;
	OFX::DoubleParam* _alpha;
	OFX::BooleanParam* _invert;
	EGammaType getGammaType() const { return static_cast<EGammaType>( _gammaType->getValue() ); }
};

}
}
}

#endif
