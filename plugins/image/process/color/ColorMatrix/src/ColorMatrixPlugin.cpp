#include "ColorMatrixPlugin.hpp"
#include "ColorMatrixProcess.hpp"
#include "ColorMatrixDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorMatrix {


ColorMatrixPlugin::ColorMatrixPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_redMix   = fetchRGBAParam( kRedMixedValues );
	_greenMix = fetchRGBAParam( kGreenMixedValues );
	_blueMix  = fetchRGBAParam( kBlueMixedValues );
	_alphaMix = fetchRGBAParam( kAlphaMixedValues );
}

ColorMatrixProcessParams ColorMatrixPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	using namespace terry;
	ColorMatrixProcessParams params;
	OfxRGBAColourD mixValue;
	
	mixValue = _redMix->getValue();
	params.matrixMix( 0, 0 ) = mixValue.r;
	params.matrixMix( 0, 1 ) = mixValue.g;
	params.matrixMix( 0, 2 ) = mixValue.b;
	params.matrixMix( 0, 3 ) = mixValue.a;

	mixValue = _greenMix->getValue();
	params.matrixMix( 1, 0 ) = mixValue.r;
	params.matrixMix( 1, 1 ) = mixValue.g;
	params.matrixMix( 1, 2 ) = mixValue.b;
	params.matrixMix( 1, 3 ) = mixValue.a;

	mixValue = _blueMix->getValue();
	params.matrixMix( 2, 0 ) = mixValue.r;
	params.matrixMix( 2, 1 ) = mixValue.g;
	params.matrixMix( 2, 2 ) = mixValue.b;
	params.matrixMix( 2, 3 ) = mixValue.a;

	mixValue = _alphaMix->getValue();
	params.matrixMix( 3, 0 ) = mixValue.r;
	params.matrixMix( 3, 1 ) = mixValue.g;
	params.matrixMix( 3, 2 ) = mixValue.b;
	params.matrixMix( 3, 3 ) = mixValue.a;

	return params;
}

void ColorMatrixPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
}

bool ColorMatrixPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorMatrixPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<ColorMatrixProcess>( *this, args );
}


}
}
}
