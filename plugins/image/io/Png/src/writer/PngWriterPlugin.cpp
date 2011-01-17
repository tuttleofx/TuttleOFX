#include "PngWriterDefinitions.hpp"
#include "PngWriterPlugin.hpp"
#include "PngWriterProcess.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace writer {

using namespace boost::gil;

PngWriterPlugin::PngWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	_paramOutputComponents = fetchChoiceParam( kParamOutputComponents );
}

PngWriterProcessParams PngWriterPlugin::getProcessParams( const OfxTime time )
{
	PngWriterProcessParams params;

	params._filepath   = getAbsoluteFilenameAt( time );
	params._components = static_cast<EParamComponents>( this->_paramOutputComponents->getValue() );
	switch( static_cast<EParamBitDepth>( this->_paramBitDepth->getValue() ) )
	{
		case eParamBitDepth8:
			params._bitDepth = 8;
			break;
		case eParamBitDepth16:
			params._bitDepth = 16;
			break;
		default:
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user( "Incorrect bit depth." ) );
	}
	params._flip = _paramFlip->getValue();
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void PngWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );
	
	doGilRender<PngWriterProcess>( *this, args );
}

}
}
}
}
