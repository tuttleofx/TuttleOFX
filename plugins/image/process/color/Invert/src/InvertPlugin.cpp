#include "InvertPlugin.hpp"
#include "InvertProcess.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace invert {

using namespace boost::gil;
const static std::string kInvertHelpString = "<b>Image inverter</b> is used to invert components of an image.  <br />";

InvertPlugin::InvertPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_processGroup = fetchGroupParam( kParamProcessGroup );
	_processR = fetchBooleanParam( kParamProcessR );
	_processG = fetchBooleanParam( kParamProcessG );
	_processB = fetchBooleanParam( kParamProcessB );
	_processA = fetchBooleanParam( kParamProcessA );
}

InvertProcessParams InvertPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	InvertProcessParams params;

	params._red     = _processR->getValue();
	params._green     = _processG->getValue();
	params._blue     = _processB->getValue();
	params._alpha     = _processA->getValue();

	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void InvertPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth bitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent components = _clipDst->getPixelComponents();

	switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender<InvertProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		case OFX::ePixelComponentAlpha:
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

void InvertPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == "Help" )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kInvertHelpString );
	}
}

}
}
}
