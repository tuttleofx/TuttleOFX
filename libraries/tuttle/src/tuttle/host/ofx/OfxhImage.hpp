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

#ifndef _TUTTLE_HOST_OFX_IMAGE_HPP_
#define _TUTTLE_HOST_OFX_IMAGE_HPP_

#include "OfxhUtilities.hpp"
#include "attribute/OfxhClip.hpp"
#include "property/OfxhSet.hpp"

#include <tuttle/common/ofx/imageEffect.hpp>

#include <ofxImageEffect.h>

namespace tuttle {
namespace host {
namespace ofx {
namespace imageEffect {

using namespace ::tuttle::ofx::imageEffect;

// forward declarations
class OfxhImage;
class OfxhImageEffectNode;

/**
 *  instance of an image inside an image effect
 */
class OfxhImage : public property::OfxhSet
{
protected:
	/// called during ctors to get bits from the clip props into ours
	void initClipBits( attribute::OfxhClip& instance );
	static std::ptrdiff_t _count; ///< temp.... for check
	std::ptrdiff_t _id; ///< temp.... for check
	std::ptrdiff_t _referenceCount; ///< reference count on this image
	std::string _clipName; ///< for debug

public:
	// default constructor
	virtual ~OfxhImage();

	/// basic ctor, makes empty property set but sets not value
	OfxhImage();

	/// construct from a clip instance, but leave the
	/// filling it to the calling code via the propery set
	explicit OfxhImage( attribute::OfxhClip& instance );

	// Render Scale (renderScaleX,renderScaleY) -
	//
	// The proxy render scale currently being applied.
	// ------
	// Data -
	//
	// The pixel data pointer of an image.
	// ------
	// Bounds (bx1,by1,bx2,by2) -
	//
	// The bounds of an image's pixels. The bounds, in PixelCoordinates, are of the
	// addressable pixels in an image's data pointer. The order of the values is
	// x1, y1, x2, y2. X values are x1 &lt;= X &lt; x2 Y values are y1 &lt;= Y &lt; y2
	// ------
	// ROD (rodx1,rody1,rodx2,rody2) -
	//
	// The full region of definition. The ROD, in PixelCoordinates, are of the
	// addressable pixels in an image's data pointer. The order of the values is
	// x1, y1, x2, y2. X values are x1 &lt;= X &lt; x2 Y values are y1 &lt;= Y &lt; y2
	// ------
	// Row Bytes -
	//
	// The number of bytes in a row of an image.
	// ------
	// Field -
	//
	// kOfxImageFieldNone - the image is an unfielded frame
	// kOfxImageFieldBoth - the image is fielded and contains both interlaced fields
	// kOfxImageFieldLower - the image is fielded and contains a single field, being the lower field (rows 0,2,4...)
	// kOfxImageFieldUpper - the image is fielded and contains a single field, being the upper field (rows 1,3,5...)
	// ------
	// Unique Identifier -
	//
	// Uniquely labels an image. This is host set and allows a plug-in to differentiate between images. This is
	// especially useful if a plugin caches analysed information about the image (for example motion vectors). The
	// plugin can label the cached information with this identifier. If a user connects a different clip to the
	// analysed input, or the image has changed in some way then the plugin can detect this via an identifier change
	// and re-evaluate the cached information.

	// construction based on clip instance
	OfxhImage( attribute::OfxhClip& instance, // construct from clip instance taking pixel depth, components, pre mult and aspect ratio
	           double               renderScaleX,
	           double               renderScaleY,
	           void*                data,
	           const OfxRectI&      bounds,
	           const OfxRectI&      rod,
	           int                  rowBytes,
	           std::string          field,
	           std::string          uniqueIdentifier );

	// OfxImageClipHandle getHandle();

	OfxPropertySetHandle getPropHandle() const
	{
		return property::OfxhSet::getHandle();
	}

	std::string getClipName() const { return _clipName; }
	std::ptrdiff_t getId() const { return _id; }
	
	/// get the bounds of the pixels in memory
	OfxRectI getBounds() const;

	/// get the full region of this image
	OfxRectI getROD() const;

	EBitDepth getBitDepth() const;

	int getRowBytes() const;

	EPixelComponent getComponentsType() const;

	int getReference() const {  return _referenceCount; }

	void addReference( const std::size_t n = 1 ) {  _referenceCount += n; TUTTLE_TCOUT( "+"<<n<<"  Image::addReference, id:" << _id << ", ref:" << getReference() ); }
	/// release the reference count, which, if zero, deletes this
	bool releaseReference()
	{
		--_referenceCount;
		TUTTLE_TCOUT( "-  Image::releaseReference, id:" << getId() << ", ref:" << getReference() );
		if( _referenceCount < 0 )
			BOOST_THROW_EXCEPTION( std::logic_error( "Try to release an undeclared reference to an Image." ) );
		return _referenceCount <= 0;
	}

};

}
}
}
}

#endif
