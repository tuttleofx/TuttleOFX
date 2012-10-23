#include "DummyPlugin.hpp"
#include "DummyProcess.hpp"
#include "DummyDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace dummy {


DummyPlugin::DummyPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
}

DummyProcessParams<DummyPlugin::Scalar> DummyPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	DummyProcessParams<Scalar> params;
	return params;
}

void DummyPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
}

bool DummyPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	return true;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void DummyPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender<DummyProcess>( *this, args );
}


}
}
}
