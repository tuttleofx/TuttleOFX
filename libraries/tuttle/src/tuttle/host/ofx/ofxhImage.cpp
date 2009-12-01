/*
 * Software License :
 *
 * Copyright (c) 2007-2009, The Open Effects Association Ltd. All rights reserved.
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

#include <assert.h>
#include <cstdio>

// ofx
#include "ofxCore.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhClip.h"
#include "ofxhImageEffect.h"

namespace tuttle {
namespace host {
namespace ofx {
namespace imageEffect {

static Property::PropSpec imageStuffs[] = {
	{ kOfxPropType, Property::eString, 1, false, kOfxTypeImage },
	{ kOfxImageEffectPropPixelDepth, Property::eString, 1, true, kOfxBitDepthNone },
	{ kOfxImageEffectPropComponents, Property::eString, 1, true, kOfxImageComponentNone },
	{ kOfxImageEffectPropPreMultiplication, Property::eString, 1, true, kOfxImageOpaque },
	{ kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "1.0" },
	{ kOfxImagePropPixelAspectRatio, Property::eDouble, 1, true, "1.0" },
	{ kOfxImagePropData, Property::ePointer, 1, true, NULL },
	{ kOfxImagePropBounds, Property::eInt, 4, true, "0" },
	{ kOfxImagePropRegionOfDefinition, Property::eInt, 4, true, "0", },
	{ kOfxImagePropRowBytes, Property::eInt, 1, true, "0", },
	{ kOfxImagePropField, Property::eString, 1, true, "", },
	{ kOfxImagePropUniqueIdentifier, Property::eString, 1, true, "" },
	{ 0 }
};

Image::Image()
	: Property::Set( imageStuffs ),
	_referenceCount( 1 ) {}

/// called during ctor to get bits from the clip props into ours

void Image::getClipBits( attribute::ClipInstance& instance )
{
	const Property::Set& clipProperties = instance.getProps();

	// get and set the clip instance pixel depth
	const std::string& depth = clipProperties.getStringProperty( kOfxImageEffectPropPixelDepth );

	setStringProperty( kOfxImageEffectPropPixelDepth, depth );

	// get and set the clip instance components
	const std::string& comps = clipProperties.getStringProperty( kOfxImageEffectPropComponents );
	setStringProperty( kOfxImageEffectPropComponents, comps );

	// get and set the clip instance premultiplication
	setStringProperty( kOfxImageEffectPropPreMultiplication, clipProperties.getStringProperty( kOfxImageEffectPropPreMultiplication ) );

	// get and set the clip instance pixel aspect ratio
	setDoubleProperty( kOfxImagePropPixelAspectRatio, clipProperties.getDoubleProperty( kOfxImagePropPixelAspectRatio ) );

	// get and set the clip instance pixel aspect ratio
	setDoubleProperty( kOfxImagePropPixelAspectRatio, clipProperties.getDoubleProperty( kOfxImagePropPixelAspectRatio ) );
}

/// make an image from a clip instance

Image::Image( attribute::ClipInstance& instance )
	: Property::Set( imageStuffs ),
	_referenceCount( 1 )
{
	getClipBits( instance );
}

// construction based on clip instance

Image::Image( attribute::ClipInstance& instance,
              double                   renderScaleX,
              double                   renderScaleY,
              void*                    data,
              const OfxRectI&          bounds,
              const OfxRectI&          rod,
              int                      rowBytes,
              std::string              field,
              std::string              uniqueIdentifier )
	: Property::Set( imageStuffs ),
	_referenceCount( 1 )
{
	getClipBits( instance );

	// set other data
	setDoubleProperty( kOfxImageEffectPropRenderScale, renderScaleX, 0 );
	setDoubleProperty( kOfxImageEffectPropRenderScale, renderScaleY, 1 );
	setPointerProperty( kOfxImagePropData, data );
	setIntProperty( kOfxImagePropBounds, bounds.x1, 0 );
	setIntProperty( kOfxImagePropBounds, bounds.y1, 1 );
	setIntProperty( kOfxImagePropBounds, bounds.x2, 2 );
	setIntProperty( kOfxImagePropBounds, bounds.y2, 3 );
	setIntProperty( kOfxImagePropRegionOfDefinition, rod.x1, 0 );
	setIntProperty( kOfxImagePropRegionOfDefinition, rod.y1, 1 );
	setIntProperty( kOfxImagePropRegionOfDefinition, rod.x2, 2 );
	setIntProperty( kOfxImagePropRegionOfDefinition, rod.y2, 3 );
	setIntProperty( kOfxImagePropRowBytes, rowBytes );

	setStringProperty( kOfxImagePropField, field );
	setStringProperty( kOfxImageClipPropFieldOrder, field );
	setStringProperty( kOfxImagePropUniqueIdentifier, uniqueIdentifier );
}

OfxRectI Image::getBounds() const
{
	OfxRectI bounds;

	getIntPropertyN( kOfxImagePropBounds, &bounds.x1, 4 );
	return bounds;
}

OfxRectI Image::getROD() const
{
	OfxRectI rod;

	getIntPropertyN( kOfxImagePropBounds, &rod.x1, 4 );
	return rod;
}

BitDepthEnum Image::getBitDepth() const
{
	std::string depth     = getStringProperty( kOfxImageEffectPropPixelDepth );
	BitDepthEnum bitDepth = eBitDepthNone;

	if( depth == kOfxBitDepthByte )
	{
		bitDepth = eBitDepthUByte;
	}
	else if( depth == kOfxBitDepthShort )
	{
		bitDepth = eBitDepthUShort;
	}
	else if( depth == kOfxBitDepthFloat )
	{
		bitDepth = eBitDepthFloat;
	}
	return bitDepth;
}

PixelComponentEnum Image::getComponentsType() const
{
	std::string sType           = getStringProperty( kOfxImageEffectPropComponents );
	PixelComponentEnum compType = ePixelComponentNone;

	if( sType == kOfxImageComponentRGBA )
	{
		compType = ePixelComponentRGBA;
	}
	else if( sType == kOfxImageComponentAlpha )
	{
		compType = ePixelComponentAlpha;
	}
	return compType;
}

int Image::getRowBytes() const
{
	return getIntProperty( kOfxImagePropRowBytes );
}

Image::~Image() {}

// release the reference
bool Image::releaseReference()
{
	--_referenceCount;
	return _referenceCount <= 0;
}

}
}
}
}
