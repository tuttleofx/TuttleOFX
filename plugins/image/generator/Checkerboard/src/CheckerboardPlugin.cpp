#include "CheckerboardPlugin.hpp"
#include "CheckerboardProcess.hpp"
#include "CheckerboardDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace checkerboard {

CheckerboardPlugin::CheckerboardPlugin( OfxImageEffectHandle handle )
	: GeneratorPlugin( handle )
{
	_boxes   = fetchInt2DParam( kCheckerboardBoxes );
	_color1  = fetchRGBAParam( kCheckerboardColor1 );
	_color2  = fetchRGBAParam( kCheckerboardColor2 );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void CheckerboardPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<CheckerboardProcess>( *this, args );
}

void CheckerboardPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
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
