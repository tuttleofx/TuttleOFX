#include "Log2LinPlugin.hpp"
#include "Log2LinProcess.hpp"
#include "Log2LinDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorspace {
namespace log2lin {

using namespace boost::gil;

Log2LinPlugin::Log2LinPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void Log2LinPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<Log2LinProcess>( *this, args );
}

void Log2LinPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kLog2LinHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kLog2LinHelpString );
	}
}

}
}
}
}
