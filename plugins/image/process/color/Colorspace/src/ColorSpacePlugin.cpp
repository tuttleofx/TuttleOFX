#include "ColorSpacePlugin.hpp"
#include "ColorSpaceProcess.hpp"
#include "ColorSpaceDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace colorspace {

using namespace boost::gil;

ColorSpacePlugin::ColorSpacePlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_paramInGradationLaw	= fetchChoiceParam	( kColorSpaceGradationLawIn );
	_paramOutGradationLaw	= fetchChoiceParam	( kColorSpaceGradationLawOut );
	_paramInGamma		= fetchDoubleParam	( kColorSpaceInGammaValue );
	_paramOutGamma		= fetchDoubleParam	( kColorSpaceOutGammaValue );
	_paramInBlackPoint	= fetchDoubleParam	( kColorSpaceInBlackPoint );
	_paramOutBlackPoint	= fetchDoubleParam	( kColorSpaceOutBlackPoint );
	_paramInWhitePoint	= fetchDoubleParam	( kColorSpaceInWhitePoint );
	_paramOutWhitePoint	= fetchDoubleParam	( kColorSpaceOutWhitePoint );
	_paramInGammaSensito	= fetchDoubleParam	( kColorSpaceInGammaSensito );
	_paramOutGammaSensito	= fetchDoubleParam	( kColorSpaceOutGammaSensito );

	_paramInGamma		-> setIsSecret	( true );
	_paramOutGamma		-> setIsSecret	( true );
	_paramInBlackPoint	-> setIsSecret	( true );
	_paramOutBlackPoint	-> setIsSecret	( true );
	_paramInWhitePoint	-> setIsSecret	( true );
	_paramOutWhitePoint	-> setIsSecret	( true );
	_paramInGammaSensito	-> setIsSecret	( true );
	_paramOutGammaSensito	-> setIsSecret	( true );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ColorSpacePlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<ColorSpaceProcess>( *this, args );
}

void ColorSpacePlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kColorSpaceHelpButton )
	{
		sendMessage(	OFX::Message::eMessageMessage,
				"", // No XML resources
				kColorSpaceHelpString );
	}
}

}
}
}
