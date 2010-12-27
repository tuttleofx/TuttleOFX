#include "FloodFillPlugin.hpp"
#include "FloodFillProcess.hpp"
#include "FloodFillDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace floodFill {


FloodFillPlugin::FloodFillPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_paramUpperThres = fetchDoubleParam( kParamUpperThres );
	_paramLowerThres = fetchDoubleParam( kParamLowerThres );
	_paramRelativeMinMax = fetchBooleanParam( kParamMinMaxRelative );
	_paramMethod = fetchChoiceParam( kParamMethod );
}

FloodFillProcessParams<FloodFillPlugin::Scalar> FloodFillPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	FloodFillProcessParams<Scalar> params;

	params._upperThres = _paramUpperThres->getValue();
	params._lowerThres = _paramLowerThres->getValue();
	params._relativeMinMax = _paramRelativeMinMax->getValue();
	params._method = static_cast<EParamMethod>( _paramMethod->getValue() );

	return params;
}

void FloodFillPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FloodFillPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<FloodFillProcess>( *this, args );
}

}
}
}
