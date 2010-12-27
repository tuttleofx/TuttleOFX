#include "DiffPlugin.hpp"
#include "DiffProcess.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace quality {


DiffPlugin::DiffPlugin( OfxImageEffectHandle handle )
	: OFX::ImageEffect( handle )
{
	_clipSrcA      = fetchClip( kDiffSourceA );
	_clipSrcB      = fetchClip( kDiffSourceB );
	_clipDst       = fetchClip( kOfxImageEffectOutputClipName );
	_qualityMesure = fetchRGBAParam( kOutputQualityMesure );
}

DiffProcessParams DiffPlugin::getProcessParams() const
{
	DiffProcessParams params;

	return params;
}

void DiffPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
//	if( paramName == kHelpButton )
//	{
//		sendMessage( OFX::Message::eMessageMessage,
//		             "", // No XML resources
//		             kHelpString );
//	}
}

bool DiffPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	const OfxRectD irod = rectanglesIntersection( _clipSrcA->getCanonicalRod( args.time ),
	                                              _clipSrcB->getCanonicalRod( args.time ) );

	// Intersection of A & B
	rod.x1 = irod.x1;
	rod.x2 = irod.x2;
	rod.y1 = irod.y1;
	rod.y2 = irod.y2;
	return true;
}
/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void DiffPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<DiffProcess>( *this, args );
}


}
}
}
