#include "OfxhClipImage.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

/**
 * clipimage instance
 */
OfxhClipImage::OfxhClipImage( const attribute::OfxhClipImageDescriptor& desc )
	: attribute::OfxhClip( desc )
	//				, _pixelDepth( kOfxBitDepthNone )
	//				, _components( kOfxImageComponentNone )
{
	//					_par = 1.0;
	/**
	 * extra properties for the instance, these are fetched from the host
	 * via a get hook and some virtuals
	 */
	static property::OfxhPropSpec clipImageInstanceStuffs[] = {
		{ kOfxImageEffectPropPixelDepth, property::eString, 1, true, kOfxBitDepthNone },
		{ kOfxImageEffectPropComponents, property::eString, 1, true, kOfxImageComponentNone },
		{ kOfxImageClipPropUnmappedPixelDepth, property::eString, 1, true, kOfxBitDepthNone },
		{ kOfxImageClipPropUnmappedComponents, property::eString, 1, true, kOfxImageComponentNone },
		{ kOfxImageEffectPropPreMultiplication, property::eString, 1, true, kOfxImageOpaque },
		{ kOfxImagePropPixelAspectRatio, property::eDouble, 1, true, "1.0" },
		{ kOfxImageEffectPropFrameRate, property::eDouble, 1, true, "25.0" },
		{ kOfxImageEffectPropFrameRange, property::eDouble, 2, true, "0" },
		{ kOfxImageClipPropFieldOrder, property::eString, 1, true, kOfxImageFieldNone },
		{ kOfxImageEffectPropUnmappedFrameRange, property::eDouble, 2, true, "0" },
		{ kOfxImageEffectPropUnmappedFrameRate, property::eDouble, 1, true, "25.0" },
		{ kOfxImageClipPropContinuousSamples, property::eInt, 1, true, "0" },
		{ 0 },
	};

	_properties.addProperties( clipImageInstanceStuffs );
	initHook( clipImageInstanceStuffs );
}

OfxhClipImage::OfxhClipImage( const OfxhClipImage& other )
	: attribute::OfxhClip( other )
{}

/// given the colour component, find the nearest set of supported colour components

const std::string& OfxhClipImage::findSupportedComp( const std::string& s ) const
{
	static const std::string none( kOfxImageComponentNone );
	static const std::string rgba( kOfxImageComponentRGBA );
	static const std::string alpha( kOfxImageComponentAlpha );

	/// is it there
	if( isSupportedComponent( s ) )
		return s;

/// @todo tuttle: can we remove this check ?
//	/// were we fed some custom non chromatic component by getUnmappedComponents? Return it.
//	/// we should never be here mind, so a bit weird
//	if( !_effectInstance.isChromaticComponent( s ) )
//		return s;

	/// Means we have RGBA or Alpha being passed in and the clip
	/// only supports the other one, so return that
	if( s == kOfxImageComponentRGBA && isSupportedComponent( kOfxImageComponentAlpha ) )
		return alpha;

	if( s == kOfxImageComponentAlpha && isSupportedComponent( kOfxImageComponentRGBA ) )
		return rgba;

	/// wierd, must be some custom bit , if only one, choose that, otherwise no idea
	/// how to map, you need to derive to do so.
	const std::vector<std::string>& supportedComps = getSupportedComponents();
	if( supportedComps.size() == 1 )
		return supportedComps[0];

	return none;
}

}
}
}
}
