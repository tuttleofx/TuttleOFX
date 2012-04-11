#include "OpenImageIOWriterDefinitions.hpp"
#include "OpenImageIOWriterPlugin.hpp"
#include "OpenImageIOWriterProcess.hpp"

#include <imageio.h>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace writer {

OpenImageIOWriterPlugin::OpenImageIOWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	_components  = fetchChoiceParam( kTuttlePluginComponents );
	_orientation = fetchChoiceParam( kParamOutputOrientation );
	_quality     = fetchIntParam( kParamOutputQuality );
}

OpenImageIOWriterProcessParams OpenImageIOWriterPlugin::getProcessParams( const OfxTime time )
{
	using namespace OpenImageIO;
	OpenImageIOWriterProcessParams params;

	params._filepath    = getAbsoluteFilenameAt( time );
	params._components  = static_cast<ETuttlePluginComponents>( this->_components->getValue() );
	params._bitDepth    = static_cast<ETuttlePluginBitDepth>( this->_paramBitDepth->getValue() );
	params._quality     = _quality->getValue();
	params._orientation = static_cast<int>( _orientation->getValue() );
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void OpenImageIOWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );

	doGilRender<OpenImageIOWriterProcess>( *this, args );
}

}
}
}
}
