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

// ofx host
#include "OfxhBinary.hpp"
#include "OfxhImageEffectNode.hpp"
#include "property/OfxhSet.hpp"
#include "attribute/OfxhClip.hpp"

// ofx
#include <ofxCore.h>

namespace tuttle {
namespace host {
namespace ofx {
namespace imageEffect {

std::ptrdiff_t OfxhImage::_count = 0;

static property::OfxhPropSpec imageStuffs[] = {
	{ kOfxPropType, property::ePropTypeString, 1, false, kOfxTypeImage },
	{ kOfxImageEffectPropPixelDepth, property::ePropTypeString, 1, true, kOfxBitDepthNone },
	{ kOfxImageEffectPropComponents, property::ePropTypeString, 1, true, kOfxImageComponentNone },
	{ kOfxImageEffectPropPreMultiplication, property::ePropTypeString, 1, true, kOfxImageOpaque },
	{ kOfxImageEffectPropRenderScale, property::ePropTypeDouble, 2, true, "1.0" },
	{ kOfxImagePropPixelAspectRatio, property::ePropTypeDouble, 1, true, "1.0" },
	{ kOfxImagePropData, property::ePropTypePointer, 1, true, NULL },
	{ kOfxImagePropBounds, property::ePropTypeInt, 4, true, "0" },
	{ kOfxImagePropRegionOfDefinition, property::ePropTypeInt, 4, true, "0", },
	{ kOfxImagePropRowBytes, property::ePropTypeInt, 1, true, "0", },
	{ kOfxImagePropField, property::ePropTypeString, 1, true, "", },
	{ kOfxImagePropUniqueIdentifier, property::ePropTypeString, 1, true, "" },
	{ 0 }
};

OfxhImage::OfxhImage()
	: property::OfxhSet( imageStuffs )
	, _id( _count++ )
	, _referenceCount( 1 )
	, _clipName( "No clip !" )
	, _time( 0 )
{
	TUTTLE_TCOUT( "++ OfxhImage, clipName:" << getClipName() << ", time:" << getTime() << ", id:" << getId() << ", ref:" << getReferenceCount() );
}

/**
 * make an image from a clip instance
 */
OfxhImage::OfxhImage( attribute::OfxhClip& instance, const OfxTime time )
	: property::OfxhSet( imageStuffs )
	, _id( _count++ )
	, _referenceCount( 1 )
	, _clipName( instance.getName() )
	, _time( time )
{
	TUTTLE_TCOUT( "++ OfxhImage, clipName:" << getClipName() << ", time:" << getTime() << ", id:" << getId() << ", ref:" << getReferenceCount() );
	initClipBits( instance );
}

/**
 * construction based on clip instance
 */
OfxhImage::OfxhImage( attribute::OfxhClip& instance,
					  OfxTime              time,
                      double               renderScaleX,
                      double               renderScaleY,
                      void*                data,
                      const OfxRectI&      bounds,
                      const OfxRectI&      rod,
                      int                  rowBytes,
                      const std::string&   field,
                      const std::string&   uniqueIdentifier )
	: property::OfxhSet( imageStuffs )
	, _id( _count++ )
	, _referenceCount( 1 )
	, _clipName( instance.getName() )
	, _time( time )
{
	TUTTLE_TCOUT( "++ OfxhImage, clipName:" << getClipName() << ", id:" << getId() << ", ref:" << getReferenceCount() );
	initClipBits( instance );

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

OfxhImage::~OfxhImage()
{
	TUTTLE_TCOUT( "-- ~OfxhImage, clipName:" << getClipName() << ", time:" << getTime() << ", id:" << getId() << ", ref:" << getReferenceCount() );
	BOOST_ASSERT( getReferenceCount() == 1 );
}

/// called during ctor to get bits from the clip props into ours

void OfxhImage::initClipBits( attribute::OfxhClip& instance )
{
	const property::OfxhSet& clipProperties = instance.getProperties();

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
}

OfxRectI OfxhImage::getBounds() const
{
	OfxRectI bounds;
	getIntPropertyN( kOfxImagePropBounds, &bounds.x1, 4 );
	return bounds;
}

OfxRectI OfxhImage::getROD() const
{
	OfxRectI rod;
	getIntPropertyN( kOfxImagePropRegionOfDefinition, &rod.x1, 4 );
	return rod;
}

EBitDepth OfxhImage::getBitDepth() const
{
	std::string depth  = getStringProperty( kOfxImageEffectPropPixelDepth );
	EBitDepth bitDepth = eBitDepthNone;

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

EPixelComponent OfxhImage::getComponentsType() const
{
	std::string sType        = getStringProperty( kOfxImageEffectPropComponents );
	EPixelComponent compType = ePixelComponentNone;

	if( sType == kOfxImageComponentRGBA )
	{
		compType = ePixelComponentRGBA;
	}
	else if( sType == kOfxImageComponentRGB )
	{
		compType = ePixelComponentRGB;
	}
	else if( sType == kOfxImageComponentAlpha )
	{
		compType = ePixelComponentAlpha;
	}
	return compType;
}

int OfxhImage::getRowBytes() const
{
	return getIntProperty( kOfxImagePropRowBytes );
}

void OfxhImage::addReference( const std::size_t n )
{
	_referenceCount += n;
	TUTTLE_TCOUT( "+"<<n<<"  Image::addReference, id:" << getId() << ", clipName:" << getClipName() << ", time:" << getTime() << ", id:" << getId() << ", ref:" << getReferenceCount() );
}

bool OfxhImage::releaseReference()
{
	--_referenceCount;
	TUTTLE_TCOUT( "-  Image::releaseReference, id:" << getId() <<", clipName:" << getClipName() << ", time:" << getTime() << ", id:" << getId() << ", ref:" << getReferenceCount() );
	if( _referenceCount < 0 )
		BOOST_THROW_EXCEPTION( std::logic_error( "Try to release an undeclared reference to an Image." ) );
	return _referenceCount <= 0;
}

}
}
}
}
