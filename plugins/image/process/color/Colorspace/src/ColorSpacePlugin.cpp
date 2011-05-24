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

	_paramInLayout		= fetchChoiceParam	( kColorSpaceLayoutIn );
	_paramOutLayout		= fetchChoiceParam	( kColorSpaceLayoutOut );
	_paramInColorTemp	= fetchChoiceParam	( kColorSpaceTempColorIn );
	_paramOutColorTemp	= fetchChoiceParam	( kColorSpaceTempColorOut );

	updateInParams();
	updateOutParams();
}

void ColorSpacePlugin::updateInParams()
{
	switch( _paramInGradationLaw->getValue() )
	{
		case ttlc::eParamGamma :
			_paramInGamma		-> setIsSecret ( false );
			_paramInBlackPoint	-> setIsSecret ( true );
			_paramInWhitePoint	-> setIsSecret ( true );
			_paramInGammaSensito	-> setIsSecret ( true );
			break;
		case ttlc::eParamCineon :
			_paramInGamma		-> setIsSecret ( true );
			_paramInBlackPoint	-> setIsSecret ( false );
			_paramInWhitePoint	-> setIsSecret ( false );
			_paramInGammaSensito	-> setIsSecret ( false );
			break;
		default :
			_paramInGamma		-> setIsSecret ( true );
			_paramInBlackPoint	-> setIsSecret ( true );
			_paramInWhitePoint	-> setIsSecret ( true );
			_paramInGammaSensito	-> setIsSecret ( true );
			break;
	}
}

void ColorSpacePlugin::updateOutParams()
{
	switch( _paramOutGradationLaw->getValue() )
	{
		case ttlc::eParamGamma :
			_paramOutGamma		-> setIsSecret ( false );
			_paramOutBlackPoint	-> setIsSecret ( true );
			_paramOutWhitePoint	-> setIsSecret ( true );
			_paramOutGammaSensito	-> setIsSecret ( true );
			break;
		case ttlc::eParamCineon :
			_paramOutGamma		-> setIsSecret ( true );
			_paramOutBlackPoint	-> setIsSecret ( false );
			_paramOutWhitePoint	-> setIsSecret ( false );
			_paramOutGammaSensito	-> setIsSecret ( false );
			break;
		default :
			_paramOutGamma		-> setIsSecret ( true );
			_paramOutBlackPoint	-> setIsSecret ( true );
			_paramOutWhitePoint	-> setIsSecret ( true );
			_paramOutGammaSensito	-> setIsSecret ( true );
			break;
	}
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
	if( paramName == kColorSpaceGradationLawIn )
	{
		updateInParams();
	}
	if( paramName == kColorSpaceGradationLawOut )
	{
		updateOutParams();
	}
}

}
}
}
