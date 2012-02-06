#include "ConstantPlugin.hpp"
#include "ConstantProcess.hpp"
#include "ConstantDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace constant {

ConstantPlugin::ConstantPlugin( OfxImageEffectHandle handle )
	: GeneratorPlugin( handle )
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

void ConstantPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	GeneratorPlugin::getClipPreferences( clipPreferences );

	if( getExplicitConversion() == eParamGeneratorExplicitConversionAuto )
	{
		clipPreferences.setClipBitDepth( *_clipDst, OFX::eBitDepthFloat );
	}
	clipPreferences.setClipComponents( *this->_clipDst, OFX::ePixelComponentRGBA );
	clipPreferences.setPixelAspectRatio( *this->_clipDst, 1.0 );
}

}
}
}
