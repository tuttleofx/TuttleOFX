#include "ConstantPlugin.hpp"
#include "ConstantProcess.hpp"
#include "ConstantDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace constant {

ConstantPlugin::ConstantPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
        _color = fetchRGBAParam( kConstantColor );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ConstantPlugin::render( const OFX::RenderArguments& args )
{
        doGilRender<ConstantProcess>( *this, args );
}

//void ConstantPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
//{
//}

}
}
}
