
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
#include <tuttle/host/ofx/OfxhPropertySuite.hpp>
#include <tuttle/host/ofx/OfxhClip.hpp>
#include <tuttle/host/ofx/OfxhParam.hpp>
#include <tuttle/host/ofx/OfxhMemory.hpp>
#include <tuttle/host/ofx/OfxhImageEffect.hpp>
#include <tuttle/host/ofx/OfxhPluginAPICache.hpp>
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
#include <tuttle/host/ofx/OfxhHost.hpp>
#include <tuttle/host/ofx/OfxhImageEffect.hpp>
#include <tuttle/host/ofx/OfxhImageEffectAPI.hpp>

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
{
}

/// Return the rod on the clip cannoical coords!
OfxRectD ClipImage::fetchRegionOfDefinition( OfxTime time )
{
	if( !isOutput() )
	{
		if( !getConnected() )
		{
			throw exception::LogicError("fetchRegionOfDefinition on an unconnected input clip ! (clip: "+ getFullName() + ")." );
		}
		return const_cast<ClipImage*>(_connectedClip)->fetchRegionOfDefinition(time); /// @todo tuttle: hack !!!
	}

	OfxRectD rod;
	OfxPointD renderScale = { 1.0, 1.0 };
	_effect.getRegionOfDefinitionAction( time, renderScale, rod );
	return rod;
	/*
	OfxRectD rod;
	OfxPointD renderScale;

	/// @todo tuttle: strange: seams to have bug with commercial plugins (memory overflow)
	ofx::property::OfxhPropSpec inStuff[] = {
	     { kOfxPropTime, ofx::property::eDouble, 1, true, "0" },
	     { kOfxImageEffectPropRenderScale, ofx::property::eDouble, 2, true, "0" },
	     { 0 }
	};
	
	ofx::property::OfxhPropSpec outStuff[] = {
	     { kOfxImageEffectPropRegionOfDefinition, ofx::property::eDouble, 4, false, "0" },
	     { 0 }
	};
	
	ofx::property::OfxhSet inArgs(inStuff);
	ofx::property::OfxhSet outArgs(outStuff);
	
	inArgs.setDoubleProperty(kOfxPropTime,time);
	
	inArgs.setDoublePropertyN(kOfxImageEffectPropRenderScale, &renderScale.x, 2);
	
	OfxStatus stat = _effect.mainEntry(kOfxImageEffectActionGetRegionOfDefinition, (const void*)(_effect.getHandle()), &inArgs, &outArgs);
	
	if( stat == kOfxStatOK )
	{
	     outArgs.getDoublePropertyN(kOfxImageEffectPropRegionOfDefinition, &rod.x1, 4);
	}
	else if( stat == kOfxStatReplyDefault )
	{
		// Rule: default is project size
		_effect.getProjectOffset( rod.x1, rod.y1 );
		_effect.getProjectSize( rod.x2, rod.y2 );
		_effect.getRenderScaleRecursive( renderScale.x, renderScale.y );

		/// @todo tuttle: or inputs RoD if not generator ?
	}
	else
	{
		throw exception::LogicError("fetchRegionOfDefinition error on clip : " + getFullName() );
	}
	return rod;
	*/
}


/// Get the Raw Unmapped Pixel Depth from the host.
const std::string& ClipImage::getUnmappedBitDepth() const
{
	static const std::string v( _effect.getProjectBitDepth() );

	return v;
}

/// Get the Raw Unmapped Components from the host.

const std::string& ClipImage::getUnmappedComponents() const
{
	static const std::string v( _effect.getProjectPixelComponentsType() );

	return v;
}

// Frame Range (startFrame, endFrame) -
//
//  The frame range over which a clip has images.

void ClipImage::getFrameRange( double& startFrame, double& endFrame ) const
{
	startFrame = 0.0;
	endFrame   = 1.0;
}

// Unmapped Frame Range -
//
//  The unmaped frame range over which an output clip has images.
// this is applicable only to hosts and plugins that allow a plugin to change frame rates

void ClipImage::getUnmappedFrameRange( double& unmappedStartFrame, double& unmappedEndFrame ) const
{
	unmappedStartFrame = 0;
	unmappedEndFrame   = 1;
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
		if( isOutput() )
		{
			TCOUT("output already in cache !");
			TCOUT( "return output image : " << image.get() ); // << " typeid:" << typeid(image.get()).name() );
		}
		else
		{
			TCOUT( "return input image : " << image.get() ); // << " typeid:" << typeid(image.get()).name() );
		}
		*/
		return image.get();
	}
	if( isOutput() )
	{
		// make a new ref counted image
		boost::shared_ptr<Image> outputImage(new Image( *this, bounds, time ));
//		outputImage.get()->cout();
//		TCOUT( "return output image : " << outputImage.get() ); // << " typeid:" << typeid(image.get()).name() << std::endl;
		_memoryCache.put( getConnectedClipFullName(), time, outputImage );
//		TCOUT_VAR( _memoryCache.size() );
//		TCOUT( "return output image : " << _memoryCache.get( getFullName(), time ).get() );
//		_memoryCache.get( getFullName(), time ).get()->cout();
		return outputImage.get();
	}
	throw exception::LogicError("Error input clip not in cache !");
}

}
}
}

