#include "HostDescriptor.hpp"
#include "ImageEffectNode.hpp"
#include "ClipImage.hpp"
#include "Core.hpp"

// utilities
#include <tuttle/common/utils/global.hpp>
#include <tuttle/common/image/gilGlobals.hpp>

// ofx host
#include <tuttle/host/ofx/OfxhCore.hpp>
#include <tuttle/host/ofx/OfxhBinary.hpp>
#include <tuttle/host/ofx/OfxhProperty.hpp>
#include <tuttle/host/ofx/OfxhClip.hpp>
#include <tuttle/host/ofx/OfxhParam.hpp>
#include <tuttle/host/ofx/OfxhMemory.hpp>
#include <tuttle/host/ofx/OfxhPluginAPICache.hpp>
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
#include <tuttle/host/ofx/OfxhHost.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>

// boost
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

//#define _DEBUG 1

#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <cstring>

namespace tuttle {
namespace host {
namespace core {

ClipImage::ClipImage( ImageEffectNode& effect, const tuttle::host::ofx::attribute::OfxhClipImageDescriptor& desc )
: tuttle::host::ofx::attribute::OfxhClipImage( effect, desc )
, _effect( effect )
, _isConnected( false )
, _continuousSamples( false )
, _memoryCache( core::Core::instance().getMemoryCache() )
{
	_frameRange = _effect.getEffectFrameRange();
	getEditableProperties().addProperty( new ofx::property::String( "TuttleFullName", 1, 1, getFullName().c_str() ) );
}

ClipImage::~ClipImage()
{}

/// Return the rod on the clip cannoical coords!
OfxRectD ClipImage::fetchRegionOfDefinition( OfxTime time ) const
{
	if( !isOutput() )
	{
		if( !getConnected() )
		{
			throw exception::LogicError( "fetchRegionOfDefinition on an unconnected input clip ! (clip: " + getFullName() + ")." );
		}
		return _connectedClip->fetchRegionOfDefinition( time );
	}

	OfxRectD rod;
	_effect.getProperties().getDoublePropertyN( kOfxImageEffectPropRegionOfDefinition, &rod.x1, 4 );
	return rod;
}

/// Get the Raw Unmapped Pixel Depth
const std::string& ClipImage::getUnmappedBitDepth() const
{
	return getProperties().getStringProperty( kOfxImageClipPropUnmappedPixelDepth );
}

/// Get the Raw Unmapped Components from the host.

const std::string& ClipImage::getUnmappedComponents() const
{
	return getProperties().getStringProperty( kOfxImageClipPropUnmappedComponents );
}

// Frame Range (startFrame, endFrame) -
//
//  The frame range over which a clip has images.

void ClipImage::getFrameRange( double& startFrame, double& endFrame ) const
{
	startFrame = getProperties().getDoubleProperty( kOfxImageEffectPropFrameRange, 0 );
	endFrame = getProperties().getDoubleProperty( kOfxImageEffectPropFrameRange, 1 );
}

// Unmapped Frame Range -
//
//  The unmaped frame range over which an output clip has images.
// this is applicable only to hosts and plugins that allow a plugin to change frame rates

void ClipImage::getUnmappedFrameRange( double& unmappedStartFrame, double& unmappedEndFrame ) const
{
	unmappedStartFrame = getProperties().getDoubleProperty( kOfxImageEffectPropUnmappedFrameRange, 0 );
	unmappedEndFrame = getProperties().getDoubleProperty( kOfxImageEffectPropUnmappedFrameRange, 1 );
}

/// override this to fill in the image at the given time.
/// The bounds of the image on the image plane should be
/// 'appropriate', typically the value returned in getRegionsOfInterest
/// on the effect instance. Outside a render call, the optionalBounds should
/// be 'appropriate' for the.
/// If bounds is not null, fetch the indicated section of the canonical image plane.
tuttle::host::ofx::imageEffect::OfxhImage* ClipImage::getImage( OfxTime time, OfxRectD* optionalBounds )
{
	OfxRectD bounds;

	if( optionalBounds )
	{
		bounds.x1 = optionalBounds->x1;
		bounds.y1 = optionalBounds->y1;
		bounds.x2 = optionalBounds->x2;
		bounds.y2 = optionalBounds->y2;
		//		throw exception::LogicError(kOfxStatErrMissingHostFeature, "Uses optionalBounds not supported yet."); ///< @todo tuttle: this must be supported !
		//		TCOUT("on clip: " << getFullName() << " optionalBounds="<< bounds);
	}
	else
		bounds = fetchRegionOfDefinition( time );

	//	TCOUT( "--> getImage <" << getFullName() << "> connected on <" << getConnectedClipFullName() << "> with connection <" << getConnected() << "> isOutput <" << isOutput() << ">" << " bounds: " << bounds );
	boost::shared_ptr<Image> image = _memoryCache.get( getConnectedClipFullName(), time );
	//	std::cout << "got image : " << image.get() << std::endl;
	/// @todo tuttle do something with bounds... if not the same as in cache...
	if( image.get() != NULL )
	{
		/*
		 * if( isOutput() )
		 * {
		 *  TCOUT("output already in cache !");
		 *  TCOUT( "return output image : " << image.get() ); // << " typeid:" << typeid(image.get()).name() );
		 * }
		 * else
		 * {
		 *  TCOUT( "return input image : " << image.get() ); // << " typeid:" << typeid(image.get()).name() );
		 * }
		 */
		return image.get();
	}
	if( isOutput() )
	{
		// make a new ref counted image
		boost::shared_ptr<Image> outputImage( new Image( *this, bounds, time ) );
		//		outputImage.get()->cout();
		//		TCOUT( "return output image : " << outputImage.get() ); // << " typeid:" << typeid(image.get()).name() << std::endl;
		_memoryCache.put( getConnectedClipFullName(), time, outputImage );
		//		TCOUT_VAR( _memoryCache.size() );
		//		TCOUT( "return output image : " << _memoryCache.get( getFullName(), time ).get() );
		//		_memoryCache.get( getFullName(), time ).get()->cout();
		return outputImage.get();
	}
	throw exception::LogicError( "Error input clip not in cache !" );
}

}
}
}

