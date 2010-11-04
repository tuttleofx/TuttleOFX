/**
 * This plugin uses fftw3 which is GPL licensed, thus, this plugin (only) is GPL licensed.
 */

#include "IfftPlugin.hpp"
#include "IfftProcess.hpp"
#include "IfftDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/math/rectOp.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace fftTransform {
namespace ifft {

using namespace boost::gil;

IfftPlugin::IfftPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
{
	_clipSrcMod   = fetchClip( kSourcePhase );
	_clipSrcPhase = fetchClip( kSourceModule );
	_clipDst      = fetchClip( kOfxImageEffectOutputClipName );
}

OFX::Clip* IfftPlugin::getSrcClipRe() const
{
	return _clipSrcMod;
}

OFX::Clip* IfftPlugin::getSrcClipIm() const
{
	return _clipSrcPhase;
}

OFX::Clip* IfftPlugin::getDstClip() const
{
	return _clipDst;
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
	OFX::EBitDepth dstBitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();

	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				IfftProcess<rgba8_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				IfftProcess<rgba16_view_t> p( *this );
				p.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				IfftProcess<rgba32f_view_t> p( *this );
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
	/*
	   ///@todo: gray transform
	   else if( dstComponents == OFX::ePixelComponentAlpha )
	   {
	    switch( dstBitDepth )
	    {
	        case OFX::eBitDepthUByte :
	        {
	            IfftProcess<gray8_view_t> p( *this );
	            p.setupAndProcess( args );
	            break;
	        }
	        case OFX::eBitDepthUShort :
	        {
	            IfftProcess<gray16_view_t> p( *this );
	            p.setupAndProcess( args );
	            break;
	        }
	        case OFX::eBitDepthFloat :
	        {
	            IfftProcess<gray32f_view_t> p( *this );
	            p.setupAndProcess( args );
	            break;
	        }
	        case OFX::eBitDepthNone :
	            COUT_FATALERROR( "BitDepthNone not recognize." );
	            return;
	        case OFX::eBitDepthCustom :
	            COUT_FATALERROR( "BitDepthCustom not recognize." );
	            return;
	    }
	   }
	 */
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
