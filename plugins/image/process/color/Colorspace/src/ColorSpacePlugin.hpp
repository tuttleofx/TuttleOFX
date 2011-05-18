#ifndef _TUTTLE_PLUGIN_COLORSPACE_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_COLORSPACE_PLUGIN_HPP_

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>
#include <tuttle/plugin/color/colorSpaceAPI.hpp>

namespace tuttle {
namespace plugin {
namespace colorspace {

/**
 * @brief
 *
 */
class ColorSpacePlugin : public ImageEffectGilPlugin
{
public:
	ColorSpacePlugin( OfxImageEffectHandle handle );

public:
	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	const tuttle::plugin::color::EParamGradationLaw getGradationLawIn() const { return static_cast<tuttle::plugin::color::EParamGradationLaw>( _paramInGradationLaw->getValue() );}
private:
	OFX::ChoiceParam*	_paramInGradationLaw;
	OFX::ChoiceParam*	_paramOutGradationLaw;
	OFX::DoubleParam*	_paramInGamma;
	OFX::DoubleParam*	_paramOutGamma;
	OFX::DoubleParam*	_paramInBlackPoint;
	OFX::DoubleParam*	_paramOutBlackPoint;
	OFX::DoubleParam*	_paramInWhitePoint;
	OFX::DoubleParam*	_paramOutWhitePoint;
	OFX::DoubleParam*	_paramInGammaSensito;
	OFX::DoubleParam*	_paramOutGammaSensito;

};

}
}
}

#endif
