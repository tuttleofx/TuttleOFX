#include "ImageEffectNode.hpp"
#include "HostDescriptor.hpp"
#include "attribute/ClipImage.hpp"
#include "attribute/allParams.hpp"

// ofx host
#include <tuttle/host/ofx/OfxhBinary.hpp>
#include <tuttle/host/ofx/OfxhMemory.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/ofx/OfxhPluginAPICache.hpp>
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
#include <tuttle/host/ofx/OfxhHost.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>
#include <tuttle/host/ofx/property/OfxhSet.hpp>
#include <tuttle/host/ofx/attribute/OfxhClip.hpp>
#include <tuttle/host/ofx/attribute/OfxhParam.hpp>

// ofx
#include <ofxCore.h>
#include <ofxImageEffect.h>

#include <iomanip>
#include <iostream>
#include <fstream>

namespace tuttle {
namespace host {
namespace core {

// my host support code
ImageEffectNode::ImageEffectNode( tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin*         plugin,
                                  tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor& desc,
                                  const std::string&                                             context )
	: tuttle::host::ofx::imageEffect::OfxhImageEffectNode( plugin, desc, context, false )
{
	populate();
	//	createInstanceAction();
}

ImageEffectNode::ImageEffectNode( const ImageEffectNode& other )
	: tuttle::host::ofx::imageEffect::OfxhImageEffectNode( other )
{
	populate();
	createInstanceAction();
	copyAttributesValues( other );
}

ImageEffectNode::~ImageEffectNode()
{
}
void ImageEffectNode::connect( const ProcessNode& sourceEffect, ProcessAttribute& attr )
{
	const ImageEffectNode& source = dynamic_cast<const ImageEffectNode&>( sourceEffect );

	const ClipImage& output = dynamic_cast<ClipImage&>( source.getClip( kOfxImageEffectOutputClipName ) );
	ClipImage& input  = dynamic_cast<ClipImage&>( attr ); // throw an exception if not a ClipImage attribute

	input.setConnectedClip( output );
}

/**
 * @warning do a deep comparison
 */
bool ImageEffectNode::operator==( const ImageEffectNode& other ) const
{
	return ofx::imageEffect::OfxhImageEffectNode::operator==( other );
}

// get a new clip instance
tuttle::host::ofx::attribute::OfxhClipImage* ImageEffectNode::newClipImage( const tuttle::host::ofx::attribute::OfxhClipImageDescriptor& descriptor )
{
	return new ClipImage( *this, descriptor );
}

/// get default output fielding. This is passed into the clip prefs action
/// and  might be mapped (if the host allows such a thing)
const std::string& ImageEffectNode::getDefaultOutputFielding() const
{
	/// our clip is pretending to be progressive PAL SD, so return kOfxImageFieldNone
	static const std::string v( kOfxImageFieldNone );

	return v;
}

// vmessage
void ImageEffectNode::vmessage( const char* type,
                                     const char* id,
                                     const char* format,
                                     va_list     args ) const OFX_EXCEPTION_SPEC
{
	vprintf( format, args );
}

// get the project size in CANONICAL pixels, so PAL SD return 768, 576
void ImageEffectNode::getProjectSize( double& xSize, double& ySize ) const
{
	xSize = 720;
	ySize = 576;
}

// get the project offset in CANONICAL pixels, we are at 0,0
void ImageEffectNode::getProjectOffset( double& xOffset, double& yOffset ) const
{
	xOffset = 0;
	yOffset = 0;
}

// get the project extent in CANONICAL pixels, so PAL SD return 768, 576
void ImageEffectNode::getProjectExtent( double& xSize, double& ySize ) const
{
	xSize = 720.0;
	ySize = 576.0;
}

// get the PAR, SD PAL is 768/720=1.0666
double ImageEffectNode::getProjectPixelAspectRatio() const
{
	return 1.0;
}

// we are only 25 frames
double ImageEffectNode::getEffectDuration() const
{
	return 25.0;
}

// get frame rate, so progressive PAL SD return 25
double ImageEffectNode::getFrameRate() const
{
	return 25.0;
}

/// This is called whenever a param is changed by the plugin so that
/// the recursive instanceChangedAction will be fed the correct frame
double ImageEffectNode::getFrameRecursive() const
{
	return 0.0;
}

/// This is called whenever a param is changed by the plugin so that
/// the recursive instanceChangedAction will be fed the correct
/// renderScale
void ImageEffectNode::getRenderScaleRecursive( double& x, double& y ) const
{
	x = y = 1.0;
}

/**
 * The pixel components type of the current project
 * @todo tuttle: to remove in the future.... size, pixelType, BitDepth, etc... must be locally defined
 */
const std::string ImageEffectNode::getProjectPixelComponentsType() const
{
	return kOfxImageComponentRGBA;
}

/**
 * The pixel bit depth of the current project (host work in float)
 * @todo tuttle: to remove in the future.... size, pixelType, BitDepth, etc... must be locally defined
 */
const std::string ImageEffectNode::getProjectBitDepth() const
{
	//return kOfxBitDepthByte;
	return kOfxBitDepthFloat;
}

// make a parameter instance
ofx::attribute::OfxhParam* ImageEffectNode::newParam( const ofx::attribute::OfxhParamDescriptor& descriptor ) OFX_EXCEPTION_SPEC
{
	std::string name = descriptor.getName();
	try {
		if( descriptor.getParamType() == kOfxParamTypeString )
			return new ParamString( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeInteger )
			return new ParamInteger( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeDouble )
			return new ParamDouble( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeBoolean )
			return new ParamBoolean( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeChoice )
			return new ParamChoice( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeRGBA )
			return new ParamRGBA( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeRGB )
			return new ParamRGB( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeDouble2D )
			return new ParamDouble2D( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeDouble3D )
			return new ParamDouble3D( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeInteger2D )
			return new ParamInteger2D( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeInteger3D )
			return new ParamInteger3D( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypePushButton )
			return new ParamPushButton( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeGroup )
			return new ParamGroup( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypePage )
			return new ParamPage( *this, name,  descriptor );
	}
	catch( exception::LogicError& e ) // map intern exception to ofx::OfxhException
	{
		BOOST_THROW_EXCEPTION( ofx::OfxhException( e.ofxStatus(), e.what() ) );
	}
	BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatFailed, "Can't create param instance from param descriptor, type not recognized." ) );
}

void ImageEffectNode::editBegin( const std::string& name ) OFX_EXCEPTION_SPEC
{
	//BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrMissingHostFeature ) );
}

void ImageEffectNode::editEnd() OFX_EXCEPTION_SPEC
{
	//BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrMissingHostFeature ) );
}

/// Start doing progress.
void ImageEffectNode::progressStart( const std::string& message )
{
}

/// finish yer progress
void ImageEffectNode::progressEnd()
{
}

/// set the progress to some level of completion, returns
/// false if you should abandon processing, true to continue
bool ImageEffectNode::progressUpdate( double progress )
{
//	COUT( "\033[sprogress: " << std::setw(3) << int(progress * 100)  << "\033[r");
//	COUT_VAR( progress );
	return true;
}

/// get the current time on the timeline. This is not necessarily the same
/// time as being passed to an action (eg render)
double ImageEffectNode::timelineGetTime()
{
	return 0;
}

/// set the timeline to a specific time
void ImageEffectNode::timelineGotoTime( double t )
{}

/// get the first and last times available on the effect's timeline
void ImageEffectNode::timelineGetBounds( double& t1, double& t2 )
{
	t1 = 0;
	t2 = 25;
}

/// override to get frame range of the effect
void ImageEffectNode::beginRenderAction( OfxTime   startFrame,
										  OfxTime   endFrame,
										  OfxTime   step,
										  bool      interactive,
										  OfxPointD renderScale ) OFX_EXCEPTION_SPEC
{
	_frameRange.x = startFrame;
	_frameRange.y = endFrame;
	OfxhImageEffectNode::beginRenderAction( startFrame, endFrame, step, interactive, renderScale );
}

void ImageEffectNode::checkClipsConnections() const
{
	for( ClipImageMap::const_iterator it = _clips.begin();
		 it != _clips.end();
		 ++it )
	{
		const ClipImage& clip = dynamic_cast<const ClipImage&>( *(it->second) );
		if( !clip.isOutput() && !clip.getConnected() && !clip.isOptional() ) // a non optionl input clip is unconnected
		{
			BOOST_THROW_EXCEPTION( exception::LogicError( "A non optional clip is unconnected ! (" + clip.getFullName() + ")" ) );
		}
	}
}

void ImageEffectNode::initClipsFromReadsToWrites()
{
	std::string biggestBitDepth = kOfxBitDepthNone;
	std::string mostChromaticComponents = kOfxImageComponentNone;
	ClipImage& outputClip = dynamic_cast<ClipImage&>( getOutputClip() );
	bool inputClipsFound = false;

	for( ClipImageMap::iterator it = _clips.begin();
		 it != _clips.end();
		 ++it )
	{
		ClipImage& clip = dynamic_cast<ClipImage&>( *(it->second) );
		if( !clip.isOutput() && clip.getConnected() ) // filter for clip.isMask() and clip.isOptional() ?
		{
			inputClipsFound = true;
			const ClipImage& linkClip = clip.getConnectedClip();
			biggestBitDepth         = ofx::findDeepestBitDepth( linkClip.getPixelDepth(), biggestBitDepth );
			mostChromaticComponents = findMostChromaticComponents( linkClip.getComponents(), mostChromaticComponents );
		}
	}
	if( inputClipsFound && ! this->isSupportedPixelDepth( biggestBitDepth ) )
	{
		BOOST_THROW_EXCEPTION( exception::LogicError("Pixel depth " + biggestBitDepth + " not supported on plugin : " + getName() ) );
	}
	if( supportsMultipleClipDepths() )
	{
		for( ClipImageMap::iterator it = _clips.begin();
			 it != _clips.end();
			 ++it )
		{
			ClipImage& clip = dynamic_cast<ClipImage&>( *(it->second) );
			if( !clip.isOutput() && clip.getConnected() )
			{
				const ClipImage& linkClip = clip.getConnectedClip();
				const std::string& pixelDepth = linkClip.getPixelDepth();
				if( !this->isSupportedPixelDepth( pixelDepth ) )
				{
					BOOST_THROW_EXCEPTION( exception::LogicError("Pixel depth " + pixelDepth + " used by input not supported on node : " + getName()) );
				}
				clip.setPixelDepthIfNotModifiedByPlugin( pixelDepth );
			}
		}
	}
	else
	{
		for( ClipImageMap::iterator it = _clips.begin();
			 it != _clips.end();
			 ++it )
		{
			ClipImage& clip = dynamic_cast<ClipImage&>( *(it->second) );
			if( !clip.isOutput() && clip.getConnected() )
			{
				const ClipImage& linkClip = clip.getConnectedClip();
				if( ! linkClip.getNode().isSupportedPixelDepth( biggestBitDepth ) )
				{
					BOOST_THROW_EXCEPTION( exception::LogicError("Biggest pixel depth " + biggestBitDepth + " not supported on node : " + getName()) );
				}
				clip.setPixelDepthIfNotModifiedByPlugin( biggestBitDepth );
			}
		}
	}
	for( ClipImageMap::iterator it = _clips.begin();
		 it != _clips.end();
		 ++it )
	{
		ClipImage& clip = dynamic_cast<ClipImage&>( *(it->second) );
		if( !clip.isOutput() && clip.getConnected() )
		{
			const ClipImage& linkClip = clip.getConnectedClip();
			if( clip.isSupportedComponent( mostChromaticComponents ) )
				clip.setComponentsIfNotModifiedByPlugin( linkClip.getComponents() );
		}
	}
	outputClip.setPixelDepthIfNotModifiedByPlugin( biggestBitDepth );
	if( outputClip.isSupportedComponent( mostChromaticComponents ) )
		outputClip.setComponentsIfNotModifiedByPlugin( mostChromaticComponents );

	/// @todo multiple PAR
	if( supportsMultipleClipPARs() )
	{

	}
	else
	{

	}
}

void ImageEffectNode::initClipsFromWritesToReads()
{
	for( ClipImageMap::iterator it = _clips.begin();
		 it != _clips.end();
		 ++it )
	{
		ClipImage& clip = dynamic_cast<ClipImage&>( *(it->second) );
		if( !clip.isOutput() && clip.getConnected() )
		{

		}

		const ofx::property::String& propPixelDepth = clip.getProperties().fetchStringProperty( kOfxImageEffectPropPixelDepth );
		const ofx::property::String& propComponent = clip.getProperties().fetchStringProperty( kOfxImageEffectPropComponents );
		const ofx::property::Double& propPixelAspectRatio = clip.getProperties().fetchDoubleProperty( kOfxImagePropPixelAspectRatio );
		TCOUT( "-- " << "clip: " << " = " << clip.getFullName() );
		TCOUT( "-- " << kOfxImageEffectPropPixelDepth << " = " << propPixelDepth.getValue()
			<< " : " << (propPixelDepth.getModifiedBy() == ofx::property::eModifiedByPlugin ? "(plugin)" : "(host)") );
		TCOUT( "-- " << kOfxImageEffectPropComponents << " = " << propComponent.getValue()
			<< " : " << (propComponent.getModifiedBy() == ofx::property::eModifiedByPlugin ? "(plugin)" : "(host)") );
		TCOUT( "-- " << kOfxImagePropPixelAspectRatio << " = " << propPixelAspectRatio.getValue()
			<< " : " << (propPixelAspectRatio.getModifiedBy() == ofx::property::eModifiedByPlugin ? "(plugin)" : "(host)") );
	}
}


std::ostream& operator<<( std::ostream& os, const ImageEffectNode& v )
{
	os << "________________________________________________________________________________" << std::endl;
	os << "Plug-in:" << v.getLabel() << std::endl;
	os << "Description:" << v.getLongLabel() << std::endl;
	os << "Context:" << v._context << std::endl;
	os << "Clips:" << std::endl;
	for( ImageEffectNode::ClipImageMap::const_iterator it = v._clips.begin(), itEnd = v._clips.end();
	     it != itEnd;
	     ++it )
	{
		os << "  * " << it->second->getName() << std::endl;
	}
	os << "Params:" << std::endl;
	for( ImageEffectNode::ParamList::const_iterator it = v._paramList.begin(), itEnd = v._paramList.end();
	     it != itEnd;
	     ++it )
	{
		os << "  * " << it->getName() << " (" << it->getLabel() << ")" << std::endl;
	}
	os << "________________________________________________________________________________" << std::endl;
}

}
}
}
