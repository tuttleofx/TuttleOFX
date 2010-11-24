#include "DiffPlugin.hpp"
#include "DiffProcess.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

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
	using namespace boost::gil;
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth dstBitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();

	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				DiffProcess<rgba8_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				DiffProcess<rgba16_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				DiffProcess<rgba32f_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}
	else if( dstComponents == OFX::ePixelComponentAlpha )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				DiffProcess<gray8_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				DiffProcess<gray16_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				DiffProcess<gray32f_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}
}


}
}
}
