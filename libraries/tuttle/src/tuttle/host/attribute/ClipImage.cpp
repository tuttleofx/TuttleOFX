#include "ClipImage.hpp"

#include <tuttle/host/HostDescriptor.hpp>
#include <tuttle/host/Core.hpp>
#include <tuttle/host/ImageEffectNode.hpp>
#include <tuttle/host/InputBufferNode.hpp>

#include <tuttle/host/ofx/OfxhCore.hpp>
#include <tuttle/host/ofx/OfxhBinary.hpp>
#include <tuttle/host/ofx/OfxhMemory.hpp>
#include <tuttle/host/ofx/OfxhPluginAPICache.hpp>
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
#include <tuttle/host/ofx/OfxhHost.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>
#include <tuttle/host/ofx/property/OfxhSet.hpp>
#include <tuttle/host/ofx/attribute/OfxhClip.hpp>
#include <tuttle/host/ofx/attribute/OfxhParam.hpp>

#include <tuttle/common/utils/global.hpp>
#include <boost/scoped_ptr.hpp>

#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <cstring>

namespace tuttle {
namespace host {
namespace attribute {

ClipImage::ClipImage( INode& effect, const tuttle::host::ofx::attribute::OfxhClipImageDescriptor& desc )
	: Attribute( effect )
	, tuttle::host::ofx::attribute::OfxhClipImage( desc )
	, _isConnected( false )
	, _continuousSamples( false )
	, _memoryCache( Core::instance().getMemoryCache() )
{
	getEditableProperties().addProperty( new ofx::property::String( "TuttleFullName", 1, 1, getFullName().c_str() ) );
	getEditableProperties().addProperty( new ofx::property::String( "TuttleIdentifier", 1, 1, "" ) );
}

ClipImage::~ClipImage()
{}

std::string ClipImage::getFullName() const
{
	return getNode().getName() + "." + getName();
}

/// Return the rod on the clip cannoical coords!
OfxRectD ClipImage::fetchRegionOfDefinition( const OfxTime time ) const
{
	if( !isOutput() )
	{
		if( !isConnected() )
		{
			BOOST_THROW_EXCEPTION( exception::Bug()
			    << exception::dev( "fetchRegionOfDefinition on an unconnected input clip ! (clip: " + getFullName() + ")." ) );
		}
		return _connectedClip->fetchRegionOfDefinition( time );
	}

	/// @todo tuttle: renderscale, time, ?

	switch( getNode().getNodeType() )
	{
		case INode::eNodeTypeImageEffect:
			return getNode().asImageEffectNode().getRegionOfDefinition( time );
		case INode::eNodeTypeBuffer:
			return getNode().asInputBufferNode().getRegionOfDefinition( time );
		default:
			BOOST_THROW_EXCEPTION( exception::Bug()
			    << exception::dev( "fetchRegionOfDefinition unsupported on " + mapNodeTypeEnumToString(getNode().getNodeType()) + " node." ) );
	}
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

/**
 * @brief Frame Rate
 * The frame rate of a clip or instance's project.
 */
double ClipImage::getFrameRate() const
{
	return getNode().asImageEffectNode().getFrameRate();
}

// Frame Range (startFrame, endFrame) -
//
//  The frame range over which a clip has images.

void ClipImage::getFrameRange( double& startFrame, double& endFrame ) const
{
	startFrame = getProperties().getDoubleProperty( kOfxImageEffectPropFrameRange, 0 );
	endFrame   = getProperties().getDoubleProperty( kOfxImageEffectPropFrameRange, 1 );
}

/**
 * @brief Unmapped Frame Rate
 * The unmaped frame range over which an output clip has images.
 */
const double ClipImage::getUnmappedFrameRate() const
{
	return getNode().asImageEffectNode().getFrameRate();
}

// Unmapped Frame Range -
//
//  The unmaped frame range over which an output clip has images.
// this is applicable only to hosts and plugins that allow a plugin to change frame rates

void ClipImage::getUnmappedFrameRange( double& unmappedStartFrame, double& unmappedEndFrame ) const
{
	unmappedStartFrame = getProperties().getDoubleProperty( kOfxImageEffectPropUnmappedFrameRange, 0 );
	unmappedEndFrame   = getProperties().getDoubleProperty( kOfxImageEffectPropUnmappedFrameRange, 1 );
}

/// override this to fill in the image at the given time.
/// The bounds of the image on the image plane should be
/// 'appropriate', typically the value returned in getRegionsOfInterest
/// on the effect instance. Outside a render call, the optionalBounds should
/// be 'appropriate' for the.
/// If bounds is not null, fetch the indicated section of the canonical image plane.
tuttle::host::ofx::imageEffect::OfxhImage* ClipImage::getImage( const OfxTime time, const OfxRectD* optionalBounds )
{
	OfxRectD bounds;

	if( optionalBounds )
	{
		bounds.x1 = optionalBounds->x1;
		bounds.y1 = optionalBounds->y1;
		bounds.x2 = optionalBounds->x2;
		bounds.y2 = optionalBounds->y2;
		//BOOST_THROW_EXCEPTION( exception::MissingHostFeature()
		//	<< exception::dev("Uses optionalBounds not supported yet.") ); ///< @todo tuttle: needs to be supported !
		//		TCOUT("on clip: " << getFullName() << " optionalBounds="<< bounds);
	}
	else
		bounds = fetchRegionOfDefinition( time );

	//	TCOUT( "--> getImage <" << getFullName() << "> connected on <" << getConnectedClipFullName() << "> with connection <" << isConnected() << "> isOutput <" << isOutput() << ">" << " bounds: " << bounds );
	boost::shared_ptr<Image> image = _memoryCache.get( getIdentifier(), time );
	//	std::cout << "got image : " << image.get() << std::endl;
	/// @todo tuttle do something with bounds...
	/// if bounds != cache buffer bounds:
	///  * bounds < cache buffer: use rowSize to adjust, and modify pointer
	///  * bounds > cache buffer: recompute / exception ?

	return image.get();
}

}
}
}

