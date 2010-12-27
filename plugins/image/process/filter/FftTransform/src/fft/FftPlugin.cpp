/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#include "FftPlugin.hpp"
#include "FftProcess.hpp"
#include "FftDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace fft {

FftPlugin::FftPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_fftType     = fetchChoiceParam( kFftType );
	_fftTypeLong = fetchChoiceParam( kFftTypeLong );
}

FftTransformProcessParams FftPlugin::getProcessParams() const
{
	FftTransformProcessParams params;

	params._type = (EfftType)_fftType->getValue();
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void FftPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<FftProcess>( *this, args );
}

void FftPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kHelpString );
	}
	else if( paramName == kFftTypeLong && args.reason == OFX::eChangeUserEdit )
	{
		_fftType->setValue( _fftTypeLong->getValue() );
	}
	else if( paramName == kFftType && args.reason == OFX::eChangeUserEdit )
	{
		_fftTypeLong->setValue( _fftType->getValue() );
	}
}

}
}
}
}
