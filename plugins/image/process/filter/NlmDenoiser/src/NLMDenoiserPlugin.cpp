#include "NLMDenoiserPlugin.hpp"
#include "NLMDenoiserProcess.hpp"
#include "NLMDenoiserDefinitions.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace nlmDenoiser {

NLMDenoiserPlugin::NLMDenoiserPlugin( OfxImageEffectHandle handle ) :
OFX::ImageEffect( handle )
, _clipDst( 0 )
, _clipSrc( 0 )
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );
}

/**
 * Obtain a window of frames before and after the current frame.
 * This is needed because our denoiser has a video option.
 */
void NLMDenoiserPlugin::getFramesNeeded( const OFX::FramesNeededArguments &args, OFX::FramesNeededSetter &frames )
{
    OfxRangeD reqRange;
    OfxRangeD range = _clipSrc->getFrameRange( );
    const int depth = (int) fetchIntParam( kDepth )->getValue( );
    reqRange.min = args.time - depth / 2 - 1;
    reqRange.max = args.time + depth / 2 + 1;
	if( reqRange.max > range.max )
	{
		const double sub = reqRange.max - args.time;
		reqRange.min -= sub;
		reqRange.max -= sub;
	}
	if( reqRange.min < range.min )
    {
		const double add = range.min - args.time;
        reqRange.min += add;
        reqRange.max += add;
    }
    frames.setFramesNeeded( *_clipSrc, reqRange );
}


void NLMDenoiserPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	OFX::IntParam *regionRadius = fetchIntParam( kRegionRadius );
	OFX::IntParam *patchRadius = fetchIntParam( kPatchRadius );

	double margin = (regionRadius->getValue() + patchRadius->getValue()) * _clipSrc->getPixelAspectRatio();

	const OfxRectD roi = { double(args.regionOfInterest.x1 - margin),
                               double(args.regionOfInterest.y1 - margin),
                               double(args.regionOfInterest.x2 + margin + 1),
                               double(args.regionOfInterest.y2 + margin + 1)
                             };

	rois.setRegionOfInterest( *_clipSrc, roi );
}


/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void NLMDenoiserPlugin::render( const OFX::RenderArguments &args )
{
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth( );
    OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents( );
	
    // do the rendering
    switch( dstComponents )
	{
		case OFX::ePixelComponentRGBA:
		{
			switch( dstBitDepth )
			{
				case OFX::eBitDepthUByte :
				{
					NLMDenoiserProcess<boost::gil::rgba8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort :
				{
					NLMDenoiserProcess<boost::gil::rgba16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat :
				{
					NLMDenoiserProcess<boost::gil::rgba32f_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthCustom:
				case OFX::eBitDepthNone:
				{
					TUTTLE_COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
					break;
				}
			}
			break;
		}
		case OFX::ePixelComponentRGB:
		{
			switch( dstBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					NLMDenoiserProcess<boost::gil::rgb8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					NLMDenoiserProcess<boost::gil::rgb16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					NLMDenoiserProcess<boost::gil::rgb32f_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthCustom:
				case OFX::eBitDepthNone:
				{
					TUTTLE_COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
					break;
				}
			}
			break;
		}
		case OFX::ePixelComponentAlpha:
		{
			switch( dstBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					NLMDenoiserProcess<boost::gil::gray8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					NLMDenoiserProcess<boost::gil::gray16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					NLMDenoiserProcess<boost::gil::gray32f_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthCustom:
				case OFX::eBitDepthNone:
				{
					TUTTLE_COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
					break;
				}
			}
			break;
		}
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			TUTTLE_COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString(dstComponents) << ") not supported by the plugin." );
			break;
		}
	}
}

}
}
}
