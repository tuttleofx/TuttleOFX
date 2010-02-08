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

#ifndef _TUTTLE_HOST_OFX_CLIPIMAGE_HPP_
#define _TUTTLE_HOST_OFX_CLIPIMAGE_HPP_

#include "ofxImageEffect.h"
#include "OfxhClip.hpp"
#include "OfxhImage.hpp"
#include "OfxhUtilities.hpp"
#include "OfxhProperty.hpp"

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <map>
#include <stdexcept>

namespace tuttle {
namespace host {
namespace ofx {

namespace imageEffect {
class OfxhImageEffectNode;
class OfxhImageEffectNodeDescriptor;
}
namespace attribute {

/**
 *  Base to both descriptor and instance it
 * is used to basically fetch common properties
 * by function name
 */
class OfxhClipImageAccessor : virtual public attribute::OfxhClipAccessor
{
public:
	/// @brief base ctor, for a descriptor
	OfxhClipImageAccessor();

	virtual ~OfxhClipImageAccessor() = 0;

	/// @return a std::vector of supported comp
	const std::vector<std::string>& getSupportedComponents() const;

	/// @brief is the given component supported
	bool isSupportedComponent( const std::string& comp ) const;

	/// @brief does the clip do random temporal access
	bool temporalAccess() const;

	/// @brief is the clip a nominal 'mask' clip
	bool isMask() const;

	/// @brief how does this clip like fielded images to be presented to it
	const std::string& getFieldExtraction() const;

	/// @brief is the clip a nominal 'mask' clip
	bool supportsTiles() const;

	/// get a handle on the clip descriptor/instance for the C api
	virtual OfxImageClipHandle getOfxImageClipHandle() const = 0;

};

/**
 * a clip image descriptor
 */
class OfxhClipImageDescriptor : virtual public OfxhClipImageAccessor,
	public OfxhClipDescriptor
{
typedef OfxhClipImageDescriptor This;
private:
	OfxhClipImageDescriptor();
	void init( const std::string& name );
public:
	/// constructor
	OfxhClipImageDescriptor( const std::string& name );

	virtual ~OfxhClipImageDescriptor() {}

	bool operator==( const This& other ) const
	{
		if( OfxhClipDescriptor::operator!=( other ) )
			return false;
		return true;
	}
	bool operator!=( const This& other ) const { return !This::operator==(other); }

	/** get a handle on the clip descriptor for the C api
	 */
	OfxImageClipHandle getOfxImageClipHandle() const
	{
		return ( OfxImageClipHandle ) this;
	}

private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize( Archive &ar, const unsigned int version )
	{
		ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(OfxhClipDescriptor);
	}
};


/**
 * a clip image instance
 * @todo tuttle: template this class with the Node Type, so replace OfxhImageEffectNode by template parameter
 */
class OfxhClipImage : virtual public OfxhClipImageAccessor,
	public attribute::OfxhClip
{
protected:
	imageEffect::OfxhImageEffectNode& _effectInstance; ///< effect instance

public:
	OfxhClipImage( imageEffect::OfxhImageEffectNode& effectInstance, const attribute::OfxhClipImageDescriptor& desc );
	OfxhClipImage( const OfxhClipImage& other );

	virtual ~OfxhClipImage() {}

	/// @todo tuttle !!!!!!!!!!!!!!!!
	bool operator==( const OfxhClipImage& other ) const { return true; }
	bool operator!=( const OfxhClipImage& other ) const { return !operator==( other ); }

	virtual OfxhClipImage* clone() const                    = 0;
	virtual std::string    getFullName() const              = 0;
	virtual std::string    getConnectedClipFullName() const = 0; ///< @todo tuttle: remove this!

	/**
	 * get a handle on the clip descriptor for the C api
	 */
	OfxImageClipHandle getOfxImageClipHandle() const
	{
		return ( OfxImageClipHandle ) this;
	}

	/// instance changed action
	OfxStatus instanceChangedAction( std::string why,
	                                 OfxTime     time,
	                                 OfxPointD   renderScale );

	/**
	 *  Pixel Depth - fetch depth of all chromatic component in this clip
	 *
	 *   kOfxBitDepthNone (implying a clip is unconnected, not valid for an image)
	 *   kOfxBitDepthByte
	 *   kOfxBitDepthShort
	 *   kOfxBitDepthFloat
	 */
	const std::string& getPixelDepth() const
	{
		return getProperties().getStringProperty( kOfxImageEffectPropPixelDepth );
	}

	/** set the current pixel depth
	 * called by clip preferences action
	 */
	void setPixelDepth( const std::string& s )
	{
		getEditableProperties().setStringProperty( kOfxImageEffectPropPixelDepth, s );
	}

	/** Pixel Aspect Ratio
	 *  The pixel aspect ratio of a clip or image.
	 */
	const double getPixelAspectRatio() const
	{
		return getProperties().getDoubleProperty( kOfxImagePropPixelAspectRatio );
	}

	/**
	 * set the current pixel aspect ratio
	 * called by clip preferences action
	 */
	void setPixelAspectRatio( const double& s )
	{
		getEditableProperties().setDoubleProperty( kOfxImagePropPixelAspectRatio, s );
	}

	/** Components that can be fetched from this clip -
	 *
	 *  kOfxImageComponentNone (implying a clip is unconnected, not valid for an image)
	 *  kOfxImageComponentRGBA
	 *  kOfxImageComponentAlpha
	 *  and any custom ones you may think of
	 */
	const std::string& getComponents() const
	{
		return getProperties().getStringProperty( kOfxImageEffectPropComponents );
	}

	/**
	 * set the current set of components
	 * called by clip preferences action
	 */
	void setComponents( const std::string& s )
	{
		getEditableProperties().setStringProperty( kOfxImageEffectPropComponents, s );
	}

	/** Get the Raw Unmapped Pixel Depth from the host for chromatic planes
	 *
	 *  @returns
	 *     - kOfxBitDepthNone (implying a clip is unconnected image)
	 *     - kOfxBitDepthByte
	 *     - kOfxBitDepthShort
	 *     - kOfxBitDepthFloat
	 */
	virtual const std::string& getUnmappedBitDepth() const //= 0;
	{
		return getProperties().getStringProperty( kOfxImageClipPropUnmappedPixelDepth );
	}

	/** Get the Raw Unmapped Components from the host
	 *
	 *  @returns
	 *      - kOfxImageComponentNone (implying a clip is unconnected, not valid for an image)
	 *      - kOfxImageComponentRGBA
	 *      - kOfxImageComponentAlpha
	 */
	virtual const std::string& getUnmappedComponents() const //= 0;
	{
		return getProperties().getStringProperty( kOfxImageClipPropUnmappedComponents );
	}

	/** PreMultiplication -
	 *
	 *  kOfxImageOpaque - the image is opaque and so has no premultiplication state
	 *  kOfxImagePreMultiplied - the image is premultiplied by it's alpha
	 *  kOfxImageUnPreMultiplied - the image is unpremultiplied
	 */
	virtual const std::string& getPremult() const //= 0;
	{
		return getProperties().getStringProperty( kOfxImageEffectPropPreMultiplication );
	}

	/** Frame Rate -
	 *
	 *  The frame rate of a clip or instance's project.
	 */
	virtual double getFrameRate() const //= 0;
	{
		return getProperties().getDoubleProperty( kOfxImageEffectPropFrameRate );
	}

	/** Frame Range (startFrame, endFrame) -
	 *
	 *  The frame range over which a clip has images.
	 */
	virtual void getFrameRange( double& startFrame, double& endFrame ) const //= 0;
	{
		startFrame = getProperties().getDoubleProperty( kOfxImageEffectPropFrameRange, 0 );
		endFrame   = getProperties().getDoubleProperty( kOfxImageEffectPropFrameRange, 1 );
	}

	/**  Field Order - Which spatial field occurs temporally first in a frame.
	 *  @returns
	 *   - kOfxImageFieldNone - the clip material is unfielded
	 *   - kOfxImageFieldLower - the clip material is fielded, with image rows 0,2,4.... occuring first in a frame
	 *   - kOfxImageFieldUpper - the clip material is fielded, with image rows line 1,3,5.... occuring first in a frame
	 */
	virtual const std::string& getFieldOrder() const //= 0;
	{
		return getProperties().getStringProperty( kOfxImageClipPropFieldOrder );
	}

	/**
	 * @todo tuttle: This function has been added here. Why was it not before?
	 */
	virtual const std::string& getFieldExtraction() const //= 0;
	{
		return getProperties().getStringProperty( kOfxImageClipPropFieldExtraction );
	}

	/** Connected -
	 *
	 *  Says whether the clip is actually connected at the moment.
	 */
	virtual const bool getConnected() const = 0;
	//	{
	//		return getProperties().getDoubleProperty( kOfxImageClipPropConnected );
	//	}

	/** Unmapped Frame Rate -
	 *
	 *  The unmaped frame range over which an output clip has images.
	 */
	virtual const double getUnmappedFrameRate() const //= 0;
	{
		return getProperties().getDoubleProperty( kOfxImageEffectPropUnmappedFrameRate );
	}

	/** Unmapped Frame Range -
	 *
	 *  The unmaped frame range over which an output clip has images.
	 */
	virtual void getUnmappedFrameRange( double& unmappedStartFrame, double& unmappedEndFrame ) const //= 0;
	{
		unmappedStartFrame = getProperties().getDoubleProperty( kOfxImageEffectPropUnmappedFrameRange, 0 );
		unmappedEndFrame   = getProperties().getDoubleProperty( kOfxImageEffectPropUnmappedFrameRange, 1 );
	}

	/** Continuous Samples -
	 *
	 *  0 if the images can only be sampled at discreet times (eg: the clip is a sequence of frames),
	 *  1 if the images can only be sampled continuously (eg: the clip is infact an animating roto spline and can be rendered anywhen).
	 */
	virtual const bool getContinuousSamples() const //= 0;
	{
		return getProperties().getDoubleProperty( kOfxImageClipPropContinuousSamples ) != 0;
	}

	/**  override this to fill in the image at the given time.
	 *  The bounds of the image on the image plane should be
	 *  'appropriate', typically the value returned in getRegionsOfInterest
	 *  on the effect instance. Outside a render call, the optionalBounds should
	 *  be 'appropriate' for the.
	 *  If bounds is not null, fetch the indicated section of the canonical image plane.
	 */
	virtual tuttle::host::ofx::imageEffect::OfxhImage* getImage( OfxTime time, OfxRectD* optionalBounds = NULL ) = 0;

	/// override this to return the rod on the clip
	virtual OfxRectD fetchRegionOfDefinition( OfxTime time ) = 0;

	/** given the colour component, find the nearest set of supported colour components
	 *  override this for extra wierd custom component depths
	 */
	virtual const std::string& findSupportedComp( const std::string& s ) const;
};

/**
 * @brief to make ClipImageInstance clonable (for use in boost::ptr_container)
 */
inline OfxhClipImage* new_clone( const OfxhClipImage& a )
{
	return a.clone();
}

class OfxhClipImageSet //: public ClipAccessorSet
{
public:
	typedef std::map<std::string, OfxhClipImage*> ClipImageMap;
	typedef boost::ptr_vector<OfxhClipImage> ClipImageVector;

protected:
	ClipImageMap _clips; ///< clips by name
	ClipImageVector _clipsByOrder; ///< clips list
	bool _clipPrefsDirty; ///< do we need to re-run the clip prefs action

public:
	/// ctor
	///
	/// The propery set being passed in belongs to the owning
	/// plugin instance.
	explicit OfxhClipImageSet();

	explicit OfxhClipImageSet( const OfxhClipImageSet& other );

	/// dtor.
	virtual ~OfxhClipImageSet();

	bool operator==( const OfxhClipImageSet& other ) const;
	bool operator!=( const OfxhClipImageSet& other ) const { return !operator==( other ); }

	void populateClips( const imageEffect::OfxhImageEffectNodeDescriptor& descriptor ) OFX_EXCEPTION_SPEC;

	const ClipImageMap& getClips() const
	{
		return _clips;
	}

	ClipImageMap& getClips()
	{
		return _clips;
	}

	const ClipImageVector& getClipsByOrder() const
	{
		return _clipsByOrder;
	}

	ClipImageVector& getClipsByOrder()
	{
		return _clipsByOrder;
	}

	/**
	 * get the clip
	 */
	OfxhClipImage& getClip( const std::string& name ) const
	{
		ClipImageMap::const_iterator it = _clips.find( name );

		if( it == _clips.end() )
			throw OfxhException( "Clip not found (" + name + ")." );
		return *it->second;
	}

	/**
	 * add a clip
	 */
	void addClip( const std::string& name, OfxhClipImage* instance ) OFX_EXCEPTION_SPEC
	{
		if( _clips.find( name ) != _clips.end() )
			throw OfxhException( kOfxStatErrExists );
		_clips[name] = instance;
		_clipsByOrder.push_back( instance );
	}

	/**
	 * make a clip instance
	 *
	 * Client host code needs to implement this
	 */
	virtual OfxhClipImage* newClipImage( const OfxhClipImageDescriptor& descriptor ) = 0;

	/**
	 * get the nth clip, in order of declaration
	 */
	OfxhClipImage& getNthClip( int index )
	{
		return _clipsByOrder[index];
	}

	/**
	 * get the nth clip, in order of declaration
	 */
	int getNClips() const
	{
		return int(_clips.size() );
	}

	/**
	 * are the clip preferences currently dirty
	 */
	bool areClipPrefsDirty() const
	{
		return _clipPrefsDirty;
	}

private:
	void initMapFromList();
};

}
}
}
}


// force boost::is_virtual_base_of value (used by boost::serialization)
namespace boost{
template<> struct is_virtual_base_of<tuttle::host::ofx::attribute::OfxhClipDescriptor, tuttle::host::ofx::attribute::OfxhClipImageDescriptor>: public mpl::true_ {};
template<> struct is_virtual_base_of<tuttle::host::ofx::attribute::OfxhClip, tuttle::host::ofx::attribute::OfxhClipImage>: public mpl::true_ {};
}

//BOOST_CLASS_EXPORT(tuttle::host::ofx::attribute::OfxhClipImageDescriptor)

#endif
