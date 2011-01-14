#include "EXRWriterPlugin.hpp"
#include "EXRWriterProcess.hpp"
#include "EXRWriterDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

using namespace boost::gil;

EXRWriterPlugin::EXRWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	_componentsType = fetchChoiceParam( kParamComponentsType );
}

EXRWriterProcessParams EXRWriterPlugin::getProcessParams( const OfxTime time )
{
	EXRWriterProcessParams params;

	params._bitDepth       = ( EParamBitDepth ) this->_paramBitDepth->getValue();
	params._componentsType = (ECompType)_componentsType->getValue();
	params._filepath       = getAbsoluteFilenameAt( time );
	params._flip           = _paramFlip->getValue();

	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void EXRWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );

	doGilRender<EXRWriterProcess>( *this, args );
}

}
}
}
}
