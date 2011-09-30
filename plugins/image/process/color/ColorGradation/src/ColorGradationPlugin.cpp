#include "ColorGradationPlugin.hpp"
#include "ColorGradationProcess.hpp"
#include "ColorGradationDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorGradation {

ColorGradationPlugin::ColorGradationPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_paramIn  = fetchChoiceParam( kParamIn );
	_paramOut = fetchChoiceParam( kParamOut );
	_paramProcessAlpha = fetchBooleanParam( kParamProcessAlpha );
}

ColorGradationProcessParams<ColorGradationPlugin::Scalar> ColorGradationPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	ColorGradationProcessParams<Scalar> params;
	params._in  = static_cast<EParamGradation>( _paramIn->getValue() );
	params._out = static_cast<EParamGradation>( _paramOut->getValue() );
	params._processAlpha = _paramProcessAlpha->getValue();
	return params;
}

void ColorGradationPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamInvert )
	{
		int in = _paramIn->getValue();
		_paramIn->setValue( _paramOut->getValue() );
		_paramOut->setValue( in );
	}
}

bool ColorGradationPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	ColorGradationProcessParams<Scalar> params = getProcessParams();
	if( params._in == params._out )
	{
		identityClip = _clipSrc;
		identityTime = args.time;
		return true;
	}
	return false; // by default, we are not an identity operation
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorGradationPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<ColorGradationProcess>( *this, args );
}

}
}
}
