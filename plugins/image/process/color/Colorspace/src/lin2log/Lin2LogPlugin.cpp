#include "Lin2LogPlugin.hpp"
#include "Lin2LogProcess.hpp"
#include "Lin2LogDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorspace {
namespace lin2log {

Lin2LogPlugin::Lin2LogPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void Lin2LogPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<Lin2LogProcess>( *this, args );
}

void Lin2LogPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kLin2LogHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kLin2LogHelpString );
	}
}

}
}
}
}
