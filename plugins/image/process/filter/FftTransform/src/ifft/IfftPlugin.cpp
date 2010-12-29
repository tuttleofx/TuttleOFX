/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#include "IfftPlugin.hpp"
#include "IfftProcess.hpp"
#include "IfftDefinitions.hpp"

#include <tuttle/common/math/rectOp.hpp>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace ifft {

IfftPlugin::IfftPlugin( OfxImageEffectHandle handle )
	: OFX::ImageEffect( handle )
{
	_clipSrcMod   = fetchClip( kSourcePhase );
	_clipSrcPhase = fetchClip( kSourceModule );
	_clipDst      = fetchClip( kOfxImageEffectOutputClipName );
}

FftTransformProcessParams IfftPlugin::getProcessParams() const
{
	FftTransformProcessParams params;

	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void IfftPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth bitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent components = _clipDst->getPixelComponents();

	switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender<IfftProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
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

void IfftPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kHelpString );
	}
}

bool IfftPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	const OfxRectD irod = rectanglesIntersection( _clipSrcMod->getCanonicalRod( args.time ),
	                                              _clipSrcPhase->getCanonicalRod( args.time ) );

	// Intersection of A & B
	rod.x1 = irod.x1;
	rod.x2 = irod.x2;
	rod.y1 = irod.y1;
	rod.y2 = irod.y2;
	return true;
}

}
}
}
}
