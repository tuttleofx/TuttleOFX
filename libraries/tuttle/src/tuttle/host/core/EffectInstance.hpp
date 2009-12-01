/*
Software License :

Copyright (c) 2007, The Open Effects Association Ltd. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
	  contributors may be used to endorse or promote products derived from this
	  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TUTTLE_EFFECT_INSTANCE_H
#define TUTTLE_EFFECT_INSTANCE_H

#include "ProcessNode.hpp"
#include <tuttle/host/ofx/ofxhImageEffect.h>

#include <cassert>

namespace tuttle {
namespace host {
namespace core {

class EffectInstance : public ProcessNode, public tuttle::host::ofx::imageEffect::Instance
{
protected:
	OfxPointD _frameRange;
public:
	EffectInstance( tuttle::host::ofx::imageEffect::ImageEffectPlugin* plugin,
					tuttle::host::ofx::imageEffect::Descriptor& desc,
					const std::string& context );

	/**
	 * @todo a working clone...
	 */
	EffectInstance* clone() const { return new EffectInstance(_plugin, *_descriptor, _context); }

	const std::string& getName() const { return tuttle::host::ofx::imageEffect::Base::getName(); }

	void dumpToStdOut( );
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// overridden for ImageEffect::Instance

	/// get default output fielding. This is passed into the clip prefs action
	/// and  might be mapped (if the host allows such a thing)
	const std::string &getDefaultOutputFielding( ) const;

	/// make a clip
	tuttle::host::ofx::attribute::ClipImageInstance* newClipImage( tuttle::host::ofx::attribute::ClipImageDescriptor& descriptor );


	/// vmessage
	OfxStatus vmessage( const char* type,
								const char* id,
								const char* format,
								va_list args );


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
	double getProjectPixelAspectRatio( ) const;

	// The pixel components type of the current project
	const std::string getProjectPixelComponentsType( ) const;

	// The pixel bit depth of the current project
	const std::string getProjectBitDepth( ) const;

	// The duration of the effect
	// This contains the duration of the plug-in effect, in frames.
	double getEffectDuration( ) const;

	// For an instance, this is the frame rate of the project the effect is in.
	double getFrameRate( ) const;

	/// This is called whenever a param is changed by the plugin so that
	/// the recursive instanceChangedAction will be fed the correct frame
	double getFrameRecursive( ) const;

	/// This is called whenever a param is changed by the plugin so that
	/// the recursive instanceChangedAction will be fed the correct
	/// renderScale
	void getRenderScaleRecursive( double &x, double &y ) const;


	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// overridden for Param::SetInstance

	/// make a parameter instance
	///
	/// Client host code needs to implement this
	tuttle::host::ofx::attribute::ParamInstance* newParam( tuttle::host::ofx::attribute::ParamDescriptor& Descriptor );

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
	///
	/// Client host code needs to implement this
	OfxStatus editBegin( const std::string& name );

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
	///
	/// Client host code needs to implement this
	OfxStatus editEnd( );

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// overridden for Progress::ProgressI

	/// Start doing progress.
	void progressStart( const std::string &message );

	/// finish yer progress
	void progressEnd( );

	/// set the progress to some level of completion, returns
	/// false if you should abandon processing, true to continue
	bool progressUpdate( double t );

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// overridden for TimeLine::TimeLineI

	/// get the current time on the timeline. This is not necessarily the same
	/// time as being passed to an action (eg render)
	double timeLineGetTime( );

	/// set the timeline to a specific time
	void timeLineGotoTime( double t );

	/// get the first and last times available on the effect's timeline
	void timeLineGetBounds( double &t1, double &t2 );

	const OfxPointD & getEffectFrameRange( ) const
	{
		return _frameRange;
	}

	void setEffectFrameRange( const OfxPointD & range )
	{
		_frameRange = range;
	}

	void setEffectFrameRange( const double begin, const double end )
	{
		_frameRange.x = begin;
		_frameRange.y = end;
	}

	OfxStatus beginRenderAction( OfxTime startFrame,
								 OfxTime endFrame,
								 OfxTime step,
								 bool interactive,
								 OfxPointD renderScale );
};

}
}
}

#endif
