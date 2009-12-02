/*
 * Software License :
 *
 * Copyright (c) 2007-2009, The Open Effects Association Ltd.  All Rights Reserved.
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

#ifndef _TUTTLE_HOST_OFX_IMAGEEFFECT_HPP_
#define _TUTTLE_HOST_OFX_IMAGEEFFECT_HPP_

#include <tuttle/host/core/Exception.hpp>

#include "ofxCore.h"
#include "ofxImageEffect.h"

#include "ofxhHost.h"
#include "ofxhClipImage.h"
#include "ofxhProgress.h"
#include "ofxhTimeLine.h"
#include "ofxhParam.h"
#include "ofxhMemory.h"
#include "ofxhInteract.h"

namespace tuttle {
namespace host {
namespace ofx {

// forward declare
class Plugin;

namespace attribute {
class ClipDescriptor;
class ClipInstance;
}

namespace imageEffect {

// forward declare
class ImageEffectPlugin;
class OverlayInstance;
class Instance;
class Descriptor;

/// An image effect host, passed to the setHost function of all image effect plugins
class ImageEffectHost : public tuttle::host::ofx::AbstractHost
{
public:
	ImageEffectHost();
	virtual ~ImageEffectHost();

	/// fetch a suite
	virtual void* fetchSuite( const char* suiteName, int suiteVersion );

	/// Create a new instance of an image effect plug-in.
	///
	/// It is called by ImageEffectPlugin::createInstance which the
	/// client code calls when it wants to make a new instance.
	///
	///   \arg clientData - the clientData passed into the ImageEffectPlugin::createInstance
	///   \arg plugin - the plugin being created
	///   \arg desc - the descriptor for that plugin
	///   \arg context - the context to be created in
	virtual Instance* newInstance( void*              clientData,
	                               ImageEffectPlugin* plugin,
	                               Descriptor&        desc,
	                               const std::string& context ) const = 0;

	/// Function called as each plugin binary is found and loaded from disk
	///
	/// Use this in any dialogue etc... showing progress
	virtual void loadingStatus( const std::string& );

	/// Override this to filter out plugins which the host can't support for whatever reason
	///
	///   \arg plugin - the plugin to examine
	///   \arg reason - set this to report the reason the plugin was not loaded
	virtual bool pluginSupported( ImageEffectPlugin* plugin, std::string& reason ) const;

	/// Override this to create a descriptor, this makes the 'root' descriptor
	virtual Descriptor* makeDescriptor( ImageEffectPlugin* plugin ) const = 0;

	/// used to construct a context description, rootContext is the main context
	virtual Descriptor* makeDescriptor( const Descriptor& rootContext, ImageEffectPlugin* plug ) const = 0;

	/// used to construct populate the cache
	virtual Descriptor* makeDescriptor( const std::string& bundlePath, ImageEffectPlugin* plug ) const = 0;

	/// Override this to initialise an image effect descriptor after it has been
	/// created.
	virtual void initDescriptor( Descriptor* desc ) const;
};

////////////////////////////////////////////////////////////////////////////////
/// base class to both effect descriptors and instances
class Base
{
protected:
	Property::Set _properties;

public:
	Base( const Property::Set& set );
	Base( const Property::PropSpec* propSpec );
	virtual ~Base();

	/// is my magic number valid?
	virtual bool verifyMagic() { return true; }

	/// obtain a handle on this for passing to the C api
	OfxImageEffectHandle getHandle() const;

	const Property::Set& getProperties() const   { return _properties; }
	Property::Set&       getEditableProperties() { return _properties; }

	/// name of the clip
	const std::string& getShortLabel() const;

	/// name of the clip
	const std::string& getLabel() const;

	/// name of the clip
	const std::string& getName() const;

	/// name of the clip
	void setName( const std::string& name );

	/// name of the clip
	const std::string& getLongLabel() const;

	/// is the given context supported
	bool isContextSupported( const std::string& s ) const;

	/// what is the name of the group the plug-in belongs to
	const std::string& getPluginGrouping() const;

	/// is the effect single instance
	bool isSingleInstance() const;

	/// what is the thread safety on this effect
	const std::string& getRenderThreadSafety() const;

	/// should the host attempt to managed multi-threaded rendering if it can
	/// via tiling or some such
	bool getHostFrameThreading() const;

	/// get the overlay interact main entry if it exists
	OfxPluginEntryPoint* getOverlayInteractMainEntry() const;

	/// does the effect support images of differing sizes
	bool supportsMultiResolution() const;

	/// does the effect support tiled rendering
	bool supportsTiles() const;

	/// does this effect need random temporal access
	bool temporalAccess() const;

	/// is the given RGBA/A pixel depth supported by the effect
	bool isPixelDepthSupported( const std::string& s ) const;

	/// when field rendering, does the effect need to be called
	/// twice to render a frame in all circumstances (with different fields)
	bool fieldRenderTwiceAlways() const;

	/// does the effect support multiple clip depths
	bool supportsMultipleClipDepths() const;

	/// does the effect support multiple clip pixel aspect ratios
	bool supportsMultipleClipPARs() const;

	/// does changing the named param re-tigger a clip preferences action
	bool isClipPreferencesSlaveParam( const std::string& s ) const;
};

/// an image effect plugin descriptor
class Descriptor
	: public Base,
	public attribute::ParamDescriptorSet
{
private:
	// private CC
	Descriptor( const Descriptor& other )
		: Base( other._properties ),
		_plugin( other._plugin )
	{}

protected:
	Plugin* _plugin;      ///< the plugin I belong to
	std::map<std::string, attribute::ClipImageDescriptor*>  _clips;        ///< clips descriptors by name
	std::vector<attribute::ClipImageDescriptor*>            _clipsByOrder; ///< clip descriptors in order of declaration
	mutable Interact::Descriptor _overlayDescriptor; ///< descriptor to use for overlays, it has delayed description
	int _built;

public:
	/// used to construct the global description
	Descriptor( Plugin* plug );

	/// used to construct a context description, 'other' is the main context
	Descriptor( const Descriptor& rootContext, Plugin* plug );

	/// used to construct populate the cache
	Descriptor( const std::string& bundlePath, Plugin* plug );

	/// dtor
	virtual ~Descriptor();

	/// implemented for ParamDescriptorSet
	Property::Set& getParamSetProps()
	{
		return _properties;
	}

	/// get the plugin I belong to
	Plugin* getPlugin() const { return _plugin; }

	/// create a new clip and add this to the clip map
	virtual attribute::ClipImageDescriptor* defineClip( const std::string& name );

	/// get the clips
	const std::map<std::string, attribute::ClipImageDescriptor*>& getClips() const
	{
		return _clips;
	}

	/// add a new clip
	void addClip( const std::string& name, attribute::ClipImageDescriptor* clip );

	/// get the clips in order of construction
	const std::vector<attribute::ClipImageDescriptor*>& getClipsByOrder() const
	{
		return _clipsByOrder;
	}

	/// get the clips in order of construction
	std::vector<attribute::ClipImageDescriptor*>& getClipsByOrder()
	{
		return _clipsByOrder;
	}

	/// Get the interact description, this will also call describe on the interact
	/// This will return NULL if there is not main entry point or if the description failed
	/// otherwise it will return the described overlay
	Interact::Descriptor& getOverlayDescriptor( int bitDepthPerComponent = 8, bool hasAlpha = false );
};

/// a map used to specify needed frame ranges on set of clips
typedef std::map<attribute::ClipImageInstance*, std::vector<OfxRangeD> > RangeMap;

/// an image effect plugin instance.
///
/// Client code needs to filling the pure virtuals in this.
class Instance : public Base,
	public attribute::ParamInstanceSet,
	public attribute::ClipImageInstanceSet,
	public Progress::ProgressI,
	public TimeLine::TimeLineI,
	private Property::NotifyHook,
	private Property::GetHook
{
protected:
	ImageEffectPlugin* _plugin;
	std::string _context;
	Descriptor* _descriptor;
	bool _interactive;
	bool _created;
	bool _continuousSamples; ///< set by clip prefs
	bool _frameVarying; ///< set by clip prefs
	std::string _outputPreMultiplication;  ///< set by clip prefs
	std::string _outputFielding;  ///< set by clip prefs
	double _outputFrameRate; ///< set by clip prefs

public:
	/// constructor based on clip descriptor
	Instance( ImageEffectPlugin* plugin,
	          Descriptor&        other,
	          const std::string& context,
	          bool               interactive );

	/// called after construction to populate the various members
	/// ideally should be called in the ctor, but it relies on
	/// virtuals so has to be delayed until after the effect is
	/// constructed
	OfxStatus populate();
	void      populateParams( const imageEffect::Descriptor& descriptor ) throw( core::Exception );

	virtual ~Instance();

	bool operator==( const Instance& );

	/// implemented for Param::SetInstance
	virtual Property::Set& getParamSetProps();

	/// implemented for Param::SetInstance
	virtual void paramChangedByPlugin( attribute::ParamInstance* param );

	/// get the descriptor for this instance
	const Descriptor& getDescriptor() const { return *_descriptor; }

	/// return the plugin this instance was created with
	tuttle::host::ofx::imageEffect::ImageEffectPlugin* getPlugin() const { return _plugin; }

	/// return the context this instance was created with
	const std::string& getContext() const { return _context; }

	/// get the descriptor for this instance
	Descriptor& getDescriptor() { return *_descriptor; }

	/// get default output fielding. This is passed into the clip prefs action
	/// and  might be mapped (if the host allows such a thing)
	virtual const std::string& getDefaultOutputFielding() const = 0;

	/// get output fielding as set in the clip preferences action.
	const std::string& getOutputFielding() const { return _outputFielding; }

	/// get output fielding as set in the clip preferences action.
	const std::string& getOutputPreMultiplication() const { return _outputPreMultiplication; }

	/// get the output frame rate, as set in the clip prefences action.
	double getOutputFrameRate() const { return _outputFrameRate; }

	/// are all the non optional clips connected
	bool checkClipConnectionStatus() const;

	/// can this this instance render images at arbitrary times, not just frame boundaries
	/// set by getClipPreferenceAction()
	bool continuousSamples() const { return _continuousSamples; }

	/// does this instance generate a different picture on a frame change, even if the
	/// params and input images are exactly the same. eg: random noise generator
	bool isFrameVarying() const { return _frameVarying; }

	/// override this to make processing abort, return 1 to abort processing
	virtual int abort();

	/// override this to use your own memory instance - must inherrit from memory::instance
	virtual Memory::Instance* newMemoryInstance( size_t nBytes );

	// return an memory::instance calls makeMemoryInstance that can be overriden
	Memory::Instance* imageMemoryAlloc( size_t nBytes );

	/// make a clip
	//        virtual tuttle::host::ofx::attribute::ClipImageInstance* newClipImage( tuttle::host::ofx::attribute::ClipImageDescriptor* descriptor) = 0;

	virtual OfxStatus vmessage( const char* type,
	                            const char* id,
	                            const char* format,
	                            va_list     args ) const = 0;

	/// call the effect entry point
	virtual OfxStatus mainEntry( const char*    action,
	                             const void*    handle,
	                             Property::Set* inArgs,
	                             Property::Set* outArgs );

	size_t upperGetDimension( const std::string& name );

	/// overridden from Property::Notify
	virtual void notify( const std::string& name, bool singleValue, int indexOrN ) OFX_EXCEPTION_SPEC;

	/// overridden from gethook,  get the virutals for viewport size, pixel scale, background colour
	virtual double getDoubleProperty( const std::string& name, int index ) const OFX_EXCEPTION_SPEC;

	/// overridden from gethook,  get the virutals for viewport size, pixel scale, background colour
	virtual void getDoublePropertyN( const std::string& name, double* values, int count ) const OFX_EXCEPTION_SPEC;

	/// overridden from gethook, don't know what to do
	virtual void reset( const std::string& name ) OFX_EXCEPTION_SPEC;

	//// overridden from gethook
	virtual size_t getDimension( const std::string& name )  const OFX_EXCEPTION_SPEC;

	//
	// live parameters
	//

	// The size of the current project in canonical coordinates.
	// The size of a project is a sub set of the kOfxImageEffectPropProjectExtent. For example a
	// project may be a PAL SD project, but only be a letter-box within that. The project size is
	// the size of this sub window.
	virtual void getProjectSize( double& xSize, double& ySize ) const = 0;

	// The offset of the current project in canonical coordinates.
	// The offset is related to the kOfxImageEffectPropProjectSize and is the offset from the origin
	// of the project 'subwindow'. For example for a PAL SD project that is in letterbox form, the
	// project offset is the offset to the bottom left hand corner of the letter box. The project
	// offset is in canonical coordinates.
	virtual void getProjectOffset( double& xOffset, double& yOffset ) const = 0;

	// The extent of the current project in canonical coordinates.
	// The extent is the size of the 'output' for the current project. See ProjectCoordinateSystems
	// for more infomation on the project extent. The extent is in canonical coordinates and only
	// returns the top right position, as the extent is always rooted at 0,0. For example a PAL SD
	// project would have an extent of 768, 576.
	virtual void getProjectExtent( double& xSize, double& ySize ) const = 0;

	// The pixel aspect ratio of the current project
	virtual double getProjectPixelAspectRatio() const = 0;

	// The duration of the effect
	// This contains the duration of the plug-in effect, in frames.
	virtual double getEffectDuration() const = 0;

	// For an instance, this is the frame rate of the project the effect is in.
	virtual double getFrameRate() const = 0;

	/// This is called whenever a param is changed by the plugin so that
	/// the recursive instanceChangedAction will be fed the correct frame
	virtual double getFrameRecursive() const = 0;

	/// This is called whenever a param is changed by the plugin so that
	/// the recursive instanceChangedAction will be fed the correct
	/// renderScale
	virtual void getRenderScaleRecursive( double& x, double& y ) const = 0;

	/// Get whether the component is a supported 'chromatic' component (RGBA or alpha) in
	/// the base API.
	/// Override this if you have extended your chromatic colour types (eg RGB) and want
	/// the clip preferences logic to still work
	virtual bool isChromaticComponent( const std::string& str ) const;

	/// function to check for multiple bit depth support
	/// The answer will depend on host, plugin and context
	virtual bool canCurrentlyHandleMultipleClipDepths() const;

	/// calculate the default rod for this effect instance
	virtual OfxRectD calcDefaultRegionOfDefinition( OfxTime   time,
	                                                OfxPointD renderScale );

	//
	// actions
	//

	/// this is used to populate with any extra action in argumnents that may be needed
	virtual void setCustomInArgs( const std::string& action, Property::Set& inArgs );

	/// this is used to populate with any extra action out argumnents that may be needed
	virtual void setCustomOutArgs( const std::string& action, Property::Set& outArgs );

	/// this is used retrieve any out args after the action was called in mainEntry
	virtual void examineOutArgs( const std::string& action, OfxStatus stat, const Property::Set& outArgs );

	/// create an instance. This needs to be called _after_ construction and
	/// _after_ the host populates it's params and clips with the 'correct'
	/// values (either persisted ones or the defaults)
	virtual OfxStatus createInstanceAction();

	// begin/change/end instance changed
	//
	// why -
	//
	// kOfxChangeUserEdited   - the user or host changed the instance somehow and
	//                          caused a change to something, this includes undo/redos,
	//                          resets and loading values from files or presets,
	// kOfxChangePluginEdited - the plugin itself has changed the value of the instance
	//                          in some action
	// kOfxChangeTime         - the time has changed and this has affected the value
	//                          of the object because it varies over time
	//
	virtual OfxStatus beginInstanceChangedAction( const std::string& why );

	virtual OfxStatus paramInstanceChangedAction( const std::string& paramName,
	                                              const std::string& why,
	                                              OfxTime            time,
	                                              OfxPointD          renderScale );

	virtual OfxStatus clipInstanceChangedAction( const std::string& clipName,
	                                             const std::string& why,
	                                             OfxTime            time,
	                                             OfxPointD          renderScale );

	virtual OfxStatus endInstanceChangedAction( const std::string& why );

	// purge your caches
	virtual OfxStatus purgeCachesAction();

	// sync your private data
	virtual OfxStatus syncPrivateDataAction();

	// begin/end edit instance
	virtual OfxStatus beginInstanceEditAction();
	virtual OfxStatus endInstanceEditAction();

	// render action
	virtual OfxStatus beginRenderAction( OfxTime   startFrame,
	                                     OfxTime   endFrame,
	                                     OfxTime   step,
	                                     bool      interactive,
	                                     OfxPointD renderScale );

	virtual OfxStatus renderAction( OfxTime            time,
	                                const std::string& field,
	                                const OfxRectI&    renderRoI,
	                                OfxPointD          renderScale );

	virtual OfxStatus endRenderAction( OfxTime   startFrame,
	                                   OfxTime   endFrame,
	                                   OfxTime   step,
	                                   bool      interactive,
	                                   OfxPointD renderScale );

	/// Call the region of definition action the plugin at the given time
	/// and with the given render scales. The value is returned in rod.
	/// Note that if the plugin does not trap the action the default
	/// RoD is calculated and returned.
	virtual OfxStatus getRegionOfDefinitionAction( OfxTime   time,
	                                               OfxPointD renderScale,
	                                               OfxRectD& rod );

	/// call the get region of interest action on the plugin for the
	/// given frame and renderscale. The render RoI is passed in in
	/// roi, the std::map will contain the requested rois. Note
	/// That this call will check for tiling support and for
	/// default replies and set up the correct rois in these cases
	/// as well
	virtual OfxStatus getRegionOfInterestAction( OfxTime time,
	                                             OfxPointD renderScale,
	                                             const OfxRectD& roi,
	                                             std::map<attribute::ClipImageInstance*, OfxRectD>& rois );

	// get frames needed to render the given frame
	virtual OfxStatus getFrameNeededAction( OfxTime   time,
	                                        RangeMap& rangeMap );

	// is identity
	virtual OfxStatus isIdentityAction( OfxTime&           time,
	                                    const std::string& field,
	                                    const OfxRectI&    renderRoI,
	                                    OfxPointD          renderScale,
	                                    std::string&       clip );

	// time domain
	virtual OfxStatus getTimeDomainAction( OfxRangeD& range );

	/// Get the interact description, this will also call describe on the interact
	/// This will return NULL if there is not main entry point or if the description failed
	/// otherwise it will return the described overlay
	/// This is called by the CTOR of OverlayInteract to get the descriptor to do things with
	Interact::Descriptor& getOverlayDescriptor( int bitDepthPerComponent = 8, bool hasAlpha = false );

	/// Setup the default clip preferences on the clips
	virtual void setDefaultClipPreferences();

	/// Setup the default host clip instance members data retrieved from the plugin instance
	virtual void setupClipInstancePreferences( Property::Set& outArgs );

	/// Initialise the clip preferences arguments, override this to do
	/// stuff with wierd components etc... Calls setDefaultClipPreferences
	virtual void setupClipPreferencesArgs( Property::Set& args );

	/// Run the clip preferences action from the effect.
	///
	/// This will look into the input clips and output clip
	/// and set the following properties that the effect should
	/// fetch the image at.
	///     - pixel depth
	///     - components
	///     - pixel aspect ratio
	/// It will also set on the effect itselff
	///     - whether it is continuously samplable
	///     - the premult state of the output
	///     - whether the effect is frame varying
	///     - the fielding of the output clip
	///
	/// This will be run automatically by the effect in the following situations...
	///     - an input clip is changed
	///     - a clip preferences slave param is changed
	///
	/// The host still needs to call this explicitly just after the effect is wired
	/// up.
	virtual bool getClipPreferences();

	/// calls getClipPreferences only if the prefs are dirty
	///
	/// returns whether the clips prefs were dirty or not
	bool runGetClipPrefsConditionally()
	{
		if( areClipPrefsDirty() )
		{
			getClipPreferences();
			return true;
		}
		return false;
	}

	/// find the best supported bit depth for the given one. Override this if you define
	/// more depths
	virtual const std::string& bestSupportedDepth( const std::string& depth ) const;

	/// find the most chromatic components out of the two. Override this if you define
	/// more chromatic components
	virtual const std::string& findMostChromaticComponents( const std::string& a, const std::string& b ) const;
};

////////////////////////////////////////////////////////////////////////////////
/// An overlay interact for image effects, derived from one of these to
/// be an overlay interact
class OverlayInteract : public Interact::Instance
{
protected:
	/// our image effect instance
	imageEffect::Instance& _instance;

public:
	/// ctor this calls Instance->getOverlayDescriptor to get the descriptor
	OverlayInteract( imageEffect::Instance& v, int bitDepthPerComponent = 8, bool hasAlpha = false );
};

}
}
}
}

#endif
