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
	void updateInParams();
	void updateOutParams();

public:
	ColorSpacePlugin( OfxImageEffectHandle handle );

	void render( const OFX::RenderArguments& args );
	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	const tuttle::plugin::color::EParamGradationLaw	getGradationLawIn()		const { return static_cast<tuttle::plugin::color::EParamGradationLaw>	( _paramInGradationLaw	->getValue()	);}
	const double					getGammaValueIn()		const { return static_cast<double>					( _paramInGamma		->getValue()	);}
	const double					getBlackPointValueIn()		const { return static_cast<double>					( _paramInBlackPoint	->getValue()	);}
	const double					getWhitePointValueIn()		const { return static_cast<double>					( _paramInWhitePoint	->getValue()	);}
	const double					getGammaSensitoValueIn()	const { return static_cast<double>					( _paramInGammaSensito	->getValue()	);}

	const tuttle::plugin::color::EParamGradationLaw	getGradationLawOut()		const { return static_cast<tuttle::plugin::color::EParamGradationLaw>	( _paramOutGradationLaw	->getValue()	);}
	const double					getGammaValueOut()		const { return static_cast<double>					( _paramOutGamma	->getValue()	);}
	const double					getBlackPointValueOut()		const { return static_cast<double>					( _paramOutBlackPoint	->getValue()	);}
	const double					getWhitePointValueOut()		const { return static_cast<double>					( _paramOutWhitePoint	->getValue()	);}
	const double					getGammaSensitoValueOut()	const { return static_cast<double>					( _paramOutGammaSensito	->getValue()	);}

	const tuttle::plugin::color::EParamLayout	getLayoutIn()			const { return static_cast<tuttle::plugin::color::EParamLayout>		( _paramInLayout	->getValue()	);}
	const tuttle::plugin::color::EParamLayout	getLayoutOut()			const { return static_cast<tuttle::plugin::color::EParamLayout>		( _paramOutLayout	->getValue()	);}

	const tuttle::plugin::color::EParamTemp		getTempColorIn()		const { return static_cast<tuttle::plugin::color::EParamTemp>		( _paramInColorTemp	->getValue()	);}
	const tuttle::plugin::color::EParamTemp		getTempColorOut()		const { return static_cast<tuttle::plugin::color::EParamTemp>		( _paramOutColorTemp	->getValue()	);}

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

	OFX::ChoiceParam*	_paramInLayout;
	OFX::ChoiceParam*	_paramOutLayout;
	OFX::ChoiceParam*	_paramInColorTemp;
	OFX::ChoiceParam*	_paramOutColorTemp;
};

}
}
}

#endif
