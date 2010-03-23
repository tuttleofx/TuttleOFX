/*
 * Software License :
 *
 * Copyright (c) 2007, The Open Effects Association Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <fstream>

// ofx
#include <ofxCore.h>
#include <ofxImageEffect.h>

// ofx host
#include <tuttle/host/ofx/OfxhBinary.hpp>
#include <tuttle/host/ofx/OfxhProperty.hpp>
#include <tuttle/host/ofx/OfxhClip.hpp>
#include <tuttle/host/ofx/OfxhParam.hpp>
#include <tuttle/host/ofx/OfxhMemory.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/ofx/OfxhPluginAPICache.hpp>
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
#include <tuttle/host/ofx/OfxhHost.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>

// my host
#include "HostDescriptor.hpp"
#include "ImageEffectNode.hpp"
#include "ClipImage.hpp"
#include "Param.hpp"

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

void ImageEffectNode::dumpToStdOut() const
{
	std::cout << "________________________________________________________________________________" << std::endl;
	std::cout << "Plug-in:" << this->getLabel() << std::endl;
	std::cout << "Description:" << this->getLongLabel() << std::endl;
	std::cout << "Context:" << this->_context << std::endl;
	std::cout << "Clips:" << std::endl;
	for( ClipImageMap::const_iterator it = this->_clips.begin(), itEnd = this->_clips.end();
	     it != itEnd;
	     ++it )
	{
		std::cout << "\t\t* " << it->first << std::endl;
	}
	std::cout << "Params:" << std::endl;
	for( ParamList::const_iterator it = this->_paramList.begin(), itEnd = this->_paramList.end();
	     it != itEnd;
	     ++it )
	{
		std::cout << "\t\t* " << it->getLabel() << std::endl;
	}

	std::cout << "________________________________________________________________________________" << std::endl;
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
	return kOfxBitDepthByte;
	//	return kOfxBitDepthFloat;
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
		else if( descriptor.getParamType() == kOfxParamTypeInteger2D )
			return new ParamInteger2D( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypePushButton )
			return new ParamPushButton( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypeGroup )
			return new ParamGroup( *this, name,  descriptor );
		else if( descriptor.getParamType() == kOfxParamTypePage )
			return new ParamPage( *this, name,  descriptor );
	}
	catch( exception::LogicError& e ) // map intern exception to ofx::OfxhException
	{
		throw( ofx::OfxhException( e.ofxStatus(), e.what() ) );
	}
	throw( ofx::OfxhException( kOfxStatFailed, "Can't create param instance from param descriptor, type not recognized." ) );
}

void ImageEffectNode::editBegin( const std::string& name ) OFX_EXCEPTION_SPEC
{
	//throw ofx::OfxhException( kOfxStatErrMissingHostFeature );
}

void ImageEffectNode::editEnd() OFX_EXCEPTION_SPEC
{
	//throw ofx::OfxhException( kOfxStatErrMissingHostFeature );
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
	COUT_VAR(progress);
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

}
}
}
