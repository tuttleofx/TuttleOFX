/*
Software License :

Copyright (c) 2007-2009, The Open Effects Association Ltd.  All Rights Reserved.

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

#ifndef OFXH_CLIPIMAGE_H
#define OFXH_CLIPIMAGE_H

#include "ofxhClip.h"
#include "ofxhImage.h"
#include "ofxImageEffect.h"
#include "ofxhUtilities.h"
#include "ofxhPropertySuite.h"

namespace tuttle {

namespace host {
namespace ofx {

namespace imageEffect {
class Instance;
}
namespace attribute {

/**
 *  Base to both descriptor and instance it
 * is used to basically fetch common properties
 * by function name
 */
class ClipImageAccessor : virtual public attribute::ClipAccessor
{
public:
	/// @brief base ctor, for a descriptor
	ClipImageAccessor();

	virtual ~ClipImageAccessor( )=0;

	/// @return a std::vector of supported comp
	const std::vector<std::string> &getSupportedComponents( ) const;

	/// @brief is the given component supported
	bool isSupportedComponent( const std::string &comp ) const;

	/// @brief does the clip do random temporal access
	bool temporalAccess( ) const;

	/// @brief is the clip a nominal 'mask' clip
	bool isMask( ) const;

	/// @brief how does this clip like fielded images to be presented to it
	const std::string &getFieldExtraction( ) const;

	/// @brief is the clip a nominal 'mask' clip
	bool supportsTiles( ) const;
	
    /// get a handle on the clip descriptor/instance for the C api
    virtual OfxImageClipHandle getOfxImageClipHandle( ) const = 0;

};

/**
 * a clip image descriptor
 */
class ClipImageDescriptor : virtual public ClipImageAccessor
                             , public attribute::ClipDescriptor
{
public:
	/// constructor
	ClipImageDescriptor( const std::string& name );
	virtual ~ClipImageDescriptor(){}
	/** get a handle on the clip descriptor for the C api
	 */
	OfxImageClipHandle getOfxImageClipHandle( ) const
	{
		return (OfxImageClipHandle )this;
	}
};

/**
 * a clip image instance
 */
class ClipImageInstance : virtual public ClipImageAccessor
                          , public attribute::ClipInstance
{
protected:
	imageEffect::Instance* _effectInstance; ///< image effect instance

public:
	ClipImageInstance( imageEffect::Instance* effectInstance, const attribute::ClipImageDescriptor& desc );
	virtual ~ClipImageInstance(){}

	/**
	 * get a handle on the clip descriptor for the C api
	 */
	OfxImageClipHandle getOfxImageClipHandle( ) const
	{
		return (OfxImageClipHandle )this;
	}

	/// instance changed action
	OfxStatus instanceChangedAction( std::string why,
									 OfxTime time,
									 OfxPointD renderScale );

	/**
	 *  Pixel Depth - fetch depth of all chromatic component in this clip
	 *
	 *   kOfxBitDepthNone (implying a clip is unconnected, not valid for an image)
	 *   kOfxBitDepthByte
	 *   kOfxBitDepthShort
	 *   kOfxBitDepthFloat
	 */
	const std::string &getPixelDepth( ) const
	{
		return getProperties().getStringProperty(kOfxImageEffectPropPixelDepth);
	}

	/** set the current pixel depth
	 * called by clip preferences action
	 */
	void setPixelDepth( const std::string &s )
	{
		getEditableProperties().setStringProperty( kOfxImageEffectPropPixelDepth, s );
	}

	/** Pixel Aspect Ratio
	 *  The pixel aspect ratio of a clip or image.
	 */
	const double getPixelAspectRatio( ) const
	{
		return getProperties().getDoubleProperty(kOfxImagePropPixelAspectRatio);
	}

	/**
	 * set the current pixel aspect ratio
	 * called by clip preferences action
	 */
	void setPixelAspectRatio( const double &s )
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
	const std::string &getComponents( ) const
	{
		return getProperties().getStringProperty(kOfxImageEffectPropComponents);
	}

	/**
	 * set the current set of components
	 * called by clip preferences action
	 */
	void setComponents( const std::string &s )
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
	virtual const std::string &getUnmappedBitDepth( ) const = 0;

	/** Get the Raw Unmapped Components from the host
	 *
	 *  @returns
	 *      - kOfxImageComponentNone (implying a clip is unconnected, not valid for an image)
	 *      - kOfxImageComponentRGBA
	 *      - kOfxImageComponentAlpha
	 */
	virtual const std::string &getUnmappedComponents( ) const = 0;

	/** PreMultiplication -
	 *
	 *  kOfxImageOpaque - the image is opaque and so has no premultiplication state
	 *  kOfxImagePreMultiplied - the image is premultiplied by it's alpha
	 *  kOfxImageUnPreMultiplied - the image is unpremultiplied
	 */
	virtual const std::string &getPremult( ) const = 0;

	/** Frame Rate -
	 *
	 *  The frame rate of a clip or instance's project.
	 */
	virtual double getFrameRate( ) const = 0;

	/** Frame Range (startFrame, endFrame) -
	 *
	 *  The frame range over which a clip has images.
	 */
	virtual void getFrameRange( double &startFrame, double &endFrame ) const = 0;

	/**  Field Order - Which spatial field occurs temporally first in a frame.
	 *  @returns
	 *   - kOfxImageFieldNone - the clip material is unfielded
	 *   - kOfxImageFieldLower - the clip material is fielded, with image rows 0,2,4.... occuring first in a frame
	 *   - kOfxImageFieldUpper - the clip material is fielded, with image rows line 1,3,5.... occuring first in a frame
	 */
	virtual const std::string &getFieldOrder( ) const = 0;

	/** Connected -
	 *
	 *  Says whether the clip is actually connected at the moment.
	 */
	virtual bool getConnected( ) const = 0;

	/** Unmapped Frame Rate -
	 *
	 *  The unmaped frame range over which an output clip has images.
	 */
	virtual double getUnmappedFrameRate( ) const = 0;

	/** Unmapped Frame Range -
	 *
	 *  The unmaped frame range over which an output clip has images.
	 */
	virtual void getUnmappedFrameRange( double &unmappedStartFrame, double &unmappedEndFrame ) const = 0;

	/** Continuous Samples -
	 *
	 *  0 if the images can only be sampled at discreet times (eg: the clip is a sequence of frames),
	 *  1 if the images can only be sampled continuously (eg: the clip is infact an animating roto spline and can be rendered anywhen).
	 */
	virtual bool getContinuousSamples( ) const = 0;

	/**  override this to fill in the image at the given time.
	 *  The bounds of the image on the image plane should be
	 *  'appropriate', typically the value returned in getRegionsOfInterest
	 *  on the effect instance. Outside a render call, the optionalBounds should
	 *  be 'appropriate' for the.
	 *  If bounds is not null, fetch the indicated section of the canonical image plane.
	 */
	virtual tuttle::host::ofx::imageEffect::Image* getImage( OfxTime time, OfxRectD *optionalBounds = NULL ) = 0;

	/// override this to return the rod on the clip
	virtual OfxRectD getRegionOfDefinition( OfxTime time ) const = 0;

	/** given the colour component, find the nearest set of supported colour components
	 *  override this for extra wierd custom component depths
	 */
	virtual const std::string &findSupportedComp( const std::string &s ) const;
};

}
}
}
}

#endif // OFX_CLIP_H
