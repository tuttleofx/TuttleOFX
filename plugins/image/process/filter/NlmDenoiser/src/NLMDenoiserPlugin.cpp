#include "NLMDenoiserPlugin.hpp"
#include "NLMDenoiserProcess.hpp"
#include "NLMDenoiserDefinitions.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace nlmDenoiser {


template<class T>
inline T clamp( const T& v, const T& min, const T& max )
{
	if( v < min )
		return min;
	if( v > max )
		return max;
	return v;
}

NLMDenoiserPlugin::NLMDenoiserPlugin( OfxImageEffectHandle handle )
: OFX::ImageEffect( handle )
, _clipDst( 0 )
, _clipSrc( 0 )
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );
	_paramDepth = fetchIntParam( kParamDepth );
	_paramRegionRadius = fetchIntParam( kParamRegionRadius );
	_paramPatchRadius = fetchIntParam( kParamPatchRadius );
}

/**
 * Obtain a window of frames before and after the current frame.
 * This is needed because our denoiser has a video option.
 */
void NLMDenoiserPlugin::getFramesNeeded( const OFX::FramesNeededArguments &args, OFX::FramesNeededSetter &frames )
{
    const int depth = _paramDepth->getValue();
	
    const OfxRangeD clipFullRange = _clipSrc->getFrameRange( );
//	TUTTLE_LOG_VAR2( TUTTLE_INFO, clipFullRange.min, clipFullRange.max );
	OfxRangeD requestedRange;
	requestedRange.min = args.time - depth;
	requestedRange.max = args.time + depth;
//	TUTTLE_LOG_VAR2( TUTTLE_INFO, requestedRange.min, requestedRange.max );
	OfxRangeD realRange;
	realRange.min = clamp( requestedRange.min, clipFullRange.min, clipFullRange.max );
	realRange.max = clamp( requestedRange.max, clipFullRange.min, clipFullRange.max );
//	TUTTLE_LOG_VAR2( TUTTLE_INFO, realRange.min, realRange.max );
	
    frames.setFramesNeeded( *_clipSrc, realRange );
//	TUTTLE_LOG_INFO( "NLMDenoiserPlugin::getFramesNeeded timerange min:" << realRange.min << ", max:" << realRange.max << " for time:" << args.time );
}


void NLMDenoiserPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	const double margin = ( _paramRegionRadius->getValue() + _paramPatchRadius->getValue() ) * _clipSrc->getPixelAspectRatio();

	const OfxRectD roi = {
			double(args.regionOfInterest.x1 - margin),
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
					TUTTLE_LOG_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
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
					TUTTLE_LOG_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
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
					TUTTLE_LOG_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
					break;
				}
			}
			break;
		}
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			TUTTLE_LOG_ERROR( "Pixel components (" << mapPixelComponentEnumToString(dstComponents) << ") not supported by the plugin." );
			break;
		}
	}
}

}
}
}
