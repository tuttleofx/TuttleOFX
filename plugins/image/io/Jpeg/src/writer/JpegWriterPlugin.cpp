#include "JpegWriterDefinitions.hpp"
#include "JpegWriterPlugin.hpp"
#include "JpegWriterProcess.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace writer {

JpegWriterPlugin::JpegWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	_paramPremult = fetchBooleanParam( kParamPremult );
	_paramQuality = fetchIntParam( kParamQuality );
}

JpegWriterProcessParams JpegWriterPlugin::getProcessParams( const OfxTime time )
{
	JpegWriterProcessParams params;

	params._filepath = getAbsoluteFilenameAt( time );
	params._quality  = this->_paramQuality->getValue();
	params._premult  = this->_paramPremult->getValue();
	params._flip     = this->_paramFlip->getValue();
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void JpegWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );

	doGilRender<JpegWriterProcess>( *this, args );
}

}
}
}
}
