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

#ifndef _TUTTLE_HOST_IMAGEEFFECTNODE_HPP_
#define _TUTTLE_HOST_IMAGEEFFECTNODE_HPP_

#include "ProcessNode.hpp"
#include <tuttle/host/ofx/OfxhClipImage.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/ofx/OfxhAttribute.hpp>

#include <cmath>
#include <cassert>

namespace tuttle {
namespace host {
namespace core {

class ImageEffectNode : public ProcessNode,
	public ofx::imageEffect::OfxhImageEffectNode
{
protected:
	OfxPointD _frameRange;

public:
	ImageEffectNode( ofx::imageEffect::OfxhImageEffectPlugin*         plugin,
	                 ofx::imageEffect::OfxhImageEffectNodeDescriptor& desc,
	                 const std::string&                               context );

	ImageEffectNode( const ImageEffectNode& other );

	ImageEffectNode* clone() const { return new ImageEffectNode( *this ); }

	bool operator==( const ImageEffectNode& other ) const;

	const EProcessNodeType getProcessNodeType() const { return eImageEffect; }

	void connect( const ProcessNode& sourceEffect, ProcessAttribute& attr );

	ofx::attribute::OfxhClipImage&       getOutputClip()       { return dynamic_cast<ofx::attribute::OfxhClipImage&>( getClip( kOfxImageEffectOutputClipName ) ); }
	const ofx::attribute::OfxhClipImage& getOutputClip() const { return dynamic_cast<ofx::attribute::OfxhClipImage&>( getClip( kOfxImageEffectOutputClipName ) ); }

	ProcessAttribute& getProcessAttribute( const std::string& name ) { return dynamic_cast<ProcessAttribute&>( getClip( name ) ); }
	ProcessAttribute& getSingleInputAttribute()
	{
		ofx::attribute::OfxhClipImageSet::ClipImageVector& clips = getClipsByOrder();
		ofx::attribute::OfxhClipImageSet::ClipImageMap& clipsMap = getClips();
		ofx::attribute::OfxhAttribute* inAttr;

		if( clips.size() == 1 )
		{
			inAttr = &clips[0];
		}
		else if( clips.size() > 1 )
		{
			const ofx::attribute::OfxhClipImageSet::ClipImageMap::iterator it( clipsMap.find( kOfxSimpleSourceAttributeName ) );
			if( it != clipsMap.end() )
			{
				inAttr = it->second;
			}
			else
			{
				inAttr = &clips[0];
			}
		}
		else // if( inClips.empty() )
		{
			throw exception::LogicError( "Connection failed : no clip." );
		}
		return dynamic_cast<ProcessAttribute&>( *inAttr );
	}

	const ProcessAttribute& getSingleInputAttribute() const { return const_cast<ImageEffectNode*>(this)->getSingleInputAttribute(); };

	void begin( ProcessOptions& processOptions )
	{
		//		createInstanceAction();
		getClipPreferences();
		beginRenderAction( processOptions._startFrame,
		                   processOptions._endFrame,
		                   processOptions._step,
		                   processOptions._interactive,
		                   processOptions._renderScale );
	}

	void preProcess_initialize( ProcessOptions& processOptions )
	{
		//OfxStatus status =
		getRegionOfDefinitionAction( processOptions._time,
		                             processOptions._renderScale,
		                             processOptions._renderRoD );
		processOptions._renderRoI = processOptions._renderRoD;
	}

	void preProcess_finish( ProcessOptions& processOptions )
	{
		//OfxStatus status =
		getRegionOfInterestAction( processOptions._time,
		                           processOptions._renderScale,
		                           processOptions._renderRoI,
		                           processOptions._inputsRoI );

	}

	void process( const ProcessOptions& processOptions )
	{
		OfxRectI roi = {
			floor( processOptions._renderRoI.x1 ),
			floor( processOptions._renderRoI.y1 ),
			ceil( processOptions._renderRoI.x2 ),
			ceil( processOptions._renderRoI.y2 )
		};

		renderAction( processOptions._time,
		              processOptions._field,
		              roi,
		              processOptions._renderScale );
	}

	void postProcess( ProcessOptions& processOptions )
	{}

	void end( ProcessOptions& processOptions )
	{
		endRenderAction( processOptions._startFrame,
		                 processOptions._endFrame,
		                 processOptions._step,
		                 processOptions._interactive,
		                 processOptions._renderScale );
	}

	const std::string& getName() const { return ofx::imageEffect::OfxhImageEffectNodeBase::getName(); }

	void dumpToStdOut() const;

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// overridden for imageEffect::OfxhImageEffectNode

	/// get default output fielding. This is passed into the clip prefs action
	/// and  might be mapped (if the host allows such a thing)
	const std::string& getDefaultOutputFielding() const;

	/// make a clip
	ofx::attribute::OfxhClipImage* newClipImage( const ofx::attribute::OfxhClipImageDescriptor& descriptor );

	/// vmessage
	OfxStatus vmessage( const char* type,
	                    const char* id,
	                    const char* format,
	                    va_list     args ) const;

	// The size of the current project in canonical coordinates.
	// The size of a project is a sub set of the kOfxImageEffectPropProjectExtent. For example a
	// project may be a PAL SD project, but only be a letter-box within that. The project size is
	// the size of this sub window.
	void getProjectSize( double& xSize, double& ySize ) const;

	// The offset of the current project in canonical coordinates.
	// The offset is related to the kOfxImageEffectPropProjectSize and is the offset from the origin
	// of the project 'subwindow'. For example for a PAL SD project that is in letterbox form, the
	// project offset is the offset to the bottom left hand corner of the letter box. The project
	// offset is in canonical coordinates.
	void getProjectOffset( double& xOffset, double& yOffset ) const;

	// The extent of the current project in canonical coordinates.
	// The extent is the size of the 'output' for the current project. See ProjectCoordinateSystems
	// for more infomation on the project extent. The extent is in canonical coordinates and only
	// returns the top right position, as the extent is always rooted at 0,0. For example a PAL SD
	// project would have an extent of 768, 576.
	void getProjectExtent( double& xSize, double& ySize ) const;

	// The pixel aspect ratio of the current project
	double getProjectPixelAspectRatio() const;

	// The pixel components type of the current project
	const std::string getProjectPixelComponentsType() const;

	// The pixel bit depth of the current project
	const std::string getProjectBitDepth() const;

	// The duration of the effect
	// This contains the duration of the plug-in effect, in frames.
	double getEffectDuration() const;

	// For an instance, this is the frame rate of the project the effect is in.
	double getFrameRate() const;

	/// This is called whenever a param is changed by the plugin so that
	/// the recursive instanceChangedAction will be fed the correct frame
	double getFrameRecursive() const;

	/// This is called whenever a param is changed by the plugin so that
	/// the recursive instanceChangedAction will be fed the correct
	/// renderScale
	void getRenderScaleRecursive( double& x, double& y ) const;

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// overridden for Param::SetInstance

	/// make a parameter instance
	///
	/// Client host code needs to implement this
	ofx::attribute::OfxhParam* newParam( ofx::attribute::OfxhParamDescriptor& Descriptor );

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
	///
	/// Client host code needs to implement this
	OfxStatus editBegin( const std::string& name );

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
	///
	/// Client host code needs to implement this
	OfxStatus editEnd();

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// overridden for OfxhIProgress

	/// Start doing progress.
	void progressStart( const std::string& message );

	/// finish yer progress
	void progressEnd();

	/// set the progress to some level of completion, returns
	/// false if you should abandon processing, true to continue
	bool progressUpdate( double t );

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// overridden for OfxhITimeLine

	/// get the current time on the timeline. This is not necessarily the same
	/// time as being passed to an action (eg render)
	double timelineGetTime();

	/// set the timeline to a specific time
	void timelineGotoTime( double t );

	/// get the first and last times available on the effect's timeline
	void timelineGetBounds( double& t1, double& t2 );

	const OfxPointD& getEffectFrameRange() const
	{
		return _frameRange;
	}

	void setEffectFrameRange( const OfxPointD& range )
	{
		_frameRange = range;
	}

	void setEffectFrameRange( const double begin, const double end )
	{
		_frameRange.x = begin;
		_frameRange.y = end;
	}

	OfxStatus beginRenderAction( OfxTime   startFrame,
	                             OfxTime   endFrame,
	                             OfxTime   step,
	                             bool      interactive,
	                             OfxPointD renderScale );
};

}
}
}

#endif
