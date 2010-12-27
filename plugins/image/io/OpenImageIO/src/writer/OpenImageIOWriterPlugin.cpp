#include "OpenImageIOWriterDefinitions.hpp"
#include "OpenImageIOWriterPlugin.hpp"
#include "OpenImageIOWriterProcess.hpp"

#include <OpenImageIO/imageio.h>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace writer {

OpenImageIOWriterPlugin::OpenImageIOWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	_components = fetchChoiceParam( kParamOutputComponents );
}

OpenImageIOWriterProcessParams OpenImageIOWriterPlugin::getProcessParams( const OfxTime time )
{
	OpenImageIOWriterProcessParams params;

	params._filepath   = getAbsoluteFilenameAt( time );
	params._components = static_cast<EParamComponents>( this->_components->getValue() );
	switch( static_cast<EParamBitDepth>( this->_paramBitDepth->getValue() ) )
	{
		case eParamBitDepth8:
			params._bitDepth = TypeDesc::UINT8;
			break;
		case eParamBitDepth16:
			params._bitDepth = TypeDesc::UINT16;
			break;
		case eParamBitDepth32:
			params._bitDepth = TypeDesc::FLOAT;
			break;
		default:
			BOOST_THROW_EXCEPTION( exception::Bug()
				<< exception::user() + "Incorrect bit depth param value." );
	}
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
