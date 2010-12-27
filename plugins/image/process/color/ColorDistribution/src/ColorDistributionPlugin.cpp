#include "ColorDistributionPlugin.hpp"
#include "ColorDistributionProcess.hpp"
#include "ColorDistributionDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorDistribution {

ColorDistributionPlugin::ColorDistributionPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_paramIn  = fetchChoiceParam( kParamIn );
	_paramOut = fetchChoiceParam( kParamOut );
}

ColorDistributionProcessParams<ColorDistributionPlugin::Scalar> ColorDistributionPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	ColorDistributionProcessParams<Scalar> params;
	params._in  = static_cast<EParamDistribution>( _paramIn->getValue() );
	params._out = static_cast<EParamDistribution>( _paramOut->getValue() );
	return params;
}

void ColorDistributionPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamInvert )
	{
		int in = _paramIn->getValue();
		_paramIn->setValue( _paramOut->getValue() );
		_paramOut->setValue( in );
	}
}

bool ColorDistributionPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	ColorDistributionProcessParams<Scalar> params = getProcessParams();
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
void ColorDistributionPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<ColorDistributionProcess>( *this, args );
}

}
}
}
