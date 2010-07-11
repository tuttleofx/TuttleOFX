#ifndef _TUTTLE_HOST_IMAGEEFFECTNODE_HPP_
#define _TUTTLE_HOST_IMAGEEFFECTNODE_HPP_

#include "ProcessNode.hpp"
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/ofx/attribute/OfxhAttribute.hpp>
#include <tuttle/host/ofx/attribute/OfxhClipImage.hpp>

#include <boost/numeric/conversion/cast.hpp>


namespace tuttle {
namespace host {
namespace core {

class ImageEffectNode : public ProcessNode,
	public ofx::imageEffect::OfxhImageEffectNode
{
public:
	typedef ImageEffectNode This;
protected:
	OfxPointD _frameRange;
	OfxRectD _rod;
	OfxTime _currentTime;

public:
	ImageEffectNode( ofx::imageEffect::OfxhImageEffectPlugin*         plugin,
	                 ofx::imageEffect::OfxhImageEffectNodeDescriptor& desc,
	                 const std::string&                               context );

	ImageEffectNode( const ImageEffectNode& other );
	
	~ImageEffectNode();

	const ofx::attribute::OfxhParam& getParam( const std::string& name ) const { return ofx::attribute::OfxhParamSet::getParam( name ); }
	ofx::attribute::OfxhParam& getParam( const std::string& name ) { return ofx::attribute::OfxhParamSet::getParam( name ); }

#ifndef SWIG
	ImageEffectNode* clone() const { return new ImageEffectNode( *this ); }

	bool operator==( const ImageEffectNode& other ) const;

	const EProcessNodeType getProcessNodeType() const { return eImageEffectNode; }

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
			throw( exception::LogicError( kOfxStatFailed, "Connection failed : no clip." ) );
		}
		return dynamic_cast<ProcessAttribute&>( *inAttr );
	}

	const ProcessAttribute& getSingleInputAttribute() const { return const_cast<ImageEffectNode*>(this)->getSingleInputAttribute(); };

	OfxRectD getRegionOfDefinition() const { return _rod; }

	OfxTime getCurrentTime() const { return _currentTime; }

	void begin( ProcessOptions& processOptions )
	{
		TCOUT( "begin: " << getName() );
		beginRenderAction( processOptions._startFrame,
		                   processOptions._endFrame,
		                   processOptions._step,
		                   processOptions._interactive,
		                   processOptions._renderScale );
	}

	void preProcess_finish( ProcessOptions& processOptions )
	{
		TCOUT( "preProcess_finish: " << getName() << " at time: " << processOptions._time );
		setCurrentTime( processOptions._time );

		checkClipsConnections();
		
		getClipPreferencesAction();

		initClipsFromReadsToWrites();

		OfxRectD rod;
		getRegionOfDefinitionAction( processOptions._time,
									 processOptions._renderScale,
									 rod );
		setRegionOfDefinition( rod );
		processOptions._renderRoI = rod;
		TCOUT_VAR( rod );
	}

	void preProcess_initialize( ProcessOptions& processOptions )
	{
		TCOUT( "preProcess_initialize: " << getName() );
		
		initClipsFromWritesToReads();
		
		getRegionOfInterestAction( processOptions._time,
		                           processOptions._renderScale,
		                           processOptions._renderRoI,
		                           processOptions._inputsRoI );
		TCOUT_VAR( processOptions._renderRoI );
	}

	void process( const ProcessOptions& processOptions )
	{
		TCOUT( "process: " << getName() );
		OfxRectI roi = {
			boost::numeric_cast<int>(floor( processOptions._renderRoI.x1 )),
			boost::numeric_cast<int>(floor( processOptions._renderRoI.y1 )),
			boost::numeric_cast<int>(ceil( processOptions._renderRoI.x2 )),
			boost::numeric_cast<int>(ceil( processOptions._renderRoI.y2 ))
		};

		renderAction( processOptions._time,
		              processOptions._field,
		              roi,
		              processOptions._renderScale );
	}

	void postProcess( ProcessOptions& processOptions )
	{
		TCOUT( "postProcess: " << getName() );
	}

	void end( ProcessOptions& processOptions )
	{
		TCOUT( "end: " << getName() );
		endRenderAction( processOptions._startFrame,
		                 processOptions._endFrame,
		                 processOptions._step,
		                 processOptions._interactive,
		                 processOptions._renderScale );
	}
#endif
	const std::string& getName() const { return ofx::imageEffect::OfxhImageEffectNodeBase::getName(); }

	friend std::ostream& operator<<( std::ostream& os, const This& g );

#ifdef SWIG
	%extend
	{
		ofx::attribute::OfxhParam& __getitem__( const std::string& name )
		{
			return self->getParam(name);
//			return self->getProcessAttribute(name); //< @todo tuttle: can be clip or params !
		}
		std::string __str__() const
		{
			std::stringstream s;
			s << *self;
			return s.str();
		}
	}
#endif

	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////
	// overridden for imageEffect::OfxhImageEffectNode

	/// get default output fielding. This is passed into the clip prefs action
	/// and  might be mapped (if the host allows such a thing)
	const std::string& getDefaultOutputFielding() const;

	/// make a clip
	ofx::attribute::OfxhClipImage* newClipImage( const ofx::attribute::OfxhClipImageDescriptor& descriptor );

#ifndef SWIG
	/// vmessage
	void vmessage( const char* type,
	                    const char* id,
	                    const char* format,
	                    va_list     args ) const OFX_EXCEPTION_SPEC;
#endif
	
#ifndef SWIG
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
	ofx::attribute::OfxhParam* newParam( const ofx::attribute::OfxhParamDescriptor& Descriptor ) OFX_EXCEPTION_SPEC;

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
	///
	/// Client host code needs to implement this
	void editBegin( const std::string& name ) OFX_EXCEPTION_SPEC;

	/// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
	///
	/// Client host code needs to implement this
	void editEnd() OFX_EXCEPTION_SPEC;

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

	void beginRenderAction( OfxTime   startFrame,
	                             OfxTime   endFrame,
	                             OfxTime   step,
	                             bool      interactive,
	                             OfxPointD renderScale ) OFX_EXCEPTION_SPEC;

private:

	void setRegionOfDefinition( const OfxRectD& rod )
	{
		_rod = rod;
	}

	void setCurrentTime( const OfxTime time )
	{
		_currentTime = time;
	}

	void checkClipsConnections() const;
	void initClipsFromReadsToWrites();
	void initClipsFromWritesToReads();
#endif
};

}
}
}

#endif
