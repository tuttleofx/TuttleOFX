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

#include <cassert>
#include <cstdio>

// ofx
#include "ofxCore.h"
#include "ofxClip.h"

// ofx host
#include "OfxhBinary.hpp"
#include "OfxhProperty.hpp"
#include "OfxhClip.hpp"
#include "OfxhImageEffectNode.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

////////////////////////////////////////////////////////////////////////////////
// props to clips descriptors and instances

/**
 * base ctor, for a descriptor
 */
OfxhClipImageAccessor::OfxhClipImageAccessor() {}

OfxhClipImageAccessor::~OfxhClipImageAccessor() {}

/** return a std::vector of supported comp
 */
const std::vector<std::string>& OfxhClipImageAccessor::getSupportedComponents() const
{
	return getProperties().fetchStringProperty( kOfxImageEffectPropSupportedComponents ).getValues();
}

/** is the given component supported
 */
bool OfxhClipImageAccessor::isSupportedComponent( const std::string& comp ) const
{
	return getProperties().findStringPropValueIndex( kOfxImageEffectPropSupportedComponents, comp ) != -1;
}

/** does the clip do random temporal access
 */
bool OfxhClipImageAccessor::temporalAccess() const
{
	return getProperties().getIntProperty( kOfxImageEffectPropTemporalClipAccess ) != 0;
}

/** is the clip a nominal 'mask' clip
 */
bool OfxhClipImageAccessor::isMask() const
{
	return getProperties().getIntProperty( kOfxImageClipPropIsMask ) != 0;
}

/** how does this clip like fielded images to be presented to it
 */
const std::string& OfxhClipImageAccessor::getFieldExtraction() const
{
	return getProperties().getStringProperty( kOfxImageClipPropFieldExtraction );
}

/** is the clip a nominal 'mask' clip
 */
bool OfxhClipImageAccessor::supportsTiles() const
{
	return getProperties().getIntProperty( kOfxImageEffectPropSupportsTiles ) != 0;
}

OfxhClipImageDescriptor::OfxhClipImageDescriptor()
	: tuttle::host::ofx::attribute::OfxhClipDescriptor()
{
	init("");
}

/**
 * descriptor
 */
OfxhClipImageDescriptor::OfxhClipImageDescriptor( const std::string& name )
	: tuttle::host::ofx::attribute::OfxhClipDescriptor()
{
	init(name);
}

/**
 * descriptor
 */
void OfxhClipImageDescriptor::init( const std::string& name )
{
	/// properties common to the desciptor and instance
	/// the desc and set them, the instance cannot
	static const property::OfxhPropSpec clipImageDescriptorStuffs[] = {
		{ kOfxClipPropType, property::eString, 1, true, kOfxClipTypeImage },
		{ kOfxImageEffectPropSupportedComponents, property::eString, 0, false, "" },
		{ kOfxImageEffectPropTemporalClipAccess, property::eInt, 1, false, "0" },
		{ kOfxImageClipPropIsMask, property::eInt, 1, false, "0" },
		{ kOfxImageClipPropFieldExtraction, property::eString, 1, false, kOfxImageFieldDoubled },
		{ kOfxImageEffectPropSupportsTiles, property::eInt, 1, false, "1" },
		{ 0 },
	};

	_properties.addProperties( clipImageDescriptorStuffs );
	_properties.setStringProperty( kOfxPropName, name );
}

/**
 * clip clipimage instance
 */
OfxhClipImage::OfxhClipImage( imageEffect::OfxhImageEffectNode& effectInstance, const attribute::OfxhClipImageDescriptor& desc )
: attribute::OfxhClip( desc )
, _effectInstance( effectInstance )
	//				, _pixelDepth( kOfxBitDepthNone )
	//				, _components( kOfxImageComponentNone )
{
	//					_par = 1.0;
	/**
	 * extra properties for the instance, these are fetched from the host
	 * via a get hook and some virtuals
	 */
	static property::OfxhPropSpec clipImageInstanceStuffs[] = {
		{ kOfxImageEffectPropPixelDepth, property::eString, 1, true, kOfxBitDepthNone },
		{ kOfxImageEffectPropComponents, property::eString, 1, true, kOfxImageComponentNone },
		{ kOfxImageClipPropUnmappedPixelDepth, property::eString, 1, true, kOfxBitDepthNone },
		{ kOfxImageClipPropUnmappedComponents, property::eString, 1, true, kOfxImageComponentNone },
		{ kOfxImageEffectPropPreMultiplication, property::eString, 1, true, kOfxImageOpaque },
		{ kOfxImagePropPixelAspectRatio, property::eDouble, 1, true, "1.0" },
		{ kOfxImageEffectPropFrameRate, property::eDouble, 1, true, "25.0" },
		{ kOfxImageEffectPropFrameRange, property::eDouble, 2, true, "0" },
		{ kOfxImageClipPropFieldOrder, property::eString, 1, true, kOfxImageFieldNone },
		{ kOfxImageEffectPropUnmappedFrameRange, property::eDouble, 2, true, "0" },
		{ kOfxImageEffectPropUnmappedFrameRate, property::eDouble, 1, true, "25.0" },
		{ kOfxImageClipPropContinuousSamples, property::eInt, 1, true, "0" },
		{ 0 },
	};

	_properties.addProperties( clipImageInstanceStuffs );
	initHook( clipImageInstanceStuffs );
}

OfxhClipImage::OfxhClipImage( const OfxhClipImage& other )
	: attribute::OfxhClip( other )
	, _effectInstance( other._effectInstance ) ///< @todo tuttle bad link in copy....
{}

OfxStatus OfxhClipImage::instanceChangedAction( std::string why,
                                                OfxTime     time,
                                                OfxPointD   renderScale )
{
	property::OfxhPropSpec stuff[] = {
		{ kOfxPropType, property::eString, 1, true, kOfxTypeClip },
		{ kOfxPropName, property::eString, 1, true, getName().c_str() },
		{ kOfxPropChangeReason, property::eString, 1, true, why.c_str() },
		{ kOfxPropTime, property::eDouble, 1, true, "0" },
		{ kOfxImageEffectPropRenderScale, property::eDouble, 2, true, "0" },
		{ 0 }
	};

	property::OfxhSet inArgs( stuff );

	// add the second dimension of the render scale
	inArgs.setDoubleProperty( kOfxPropTime, time );
	inArgs.setDoublePropertyN( kOfxImageEffectPropRenderScale, &renderScale.x, 2 );

	return _effectInstance.mainEntry( kOfxActionInstanceChanged, _effectInstance.getHandle(), &inArgs, 0 );
}

/// given the colour component, find the nearest set of supported colour components

const std::string& OfxhClipImage::findSupportedComp( const std::string& s ) const
{
	static const std::string none( kOfxImageComponentNone );
	static const std::string rgba( kOfxImageComponentRGBA );
	static const std::string alpha( kOfxImageComponentAlpha );

	/// is it there
	if( isSupportedComponent( s ) )
		return s;

	/// were we fed some custom non chromatic component by getUnmappedComponents? Return it.
	/// we should never be here mind, so a bit weird
	if( !_effectInstance.isChromaticComponent( s ) )
		return s;

	/// Means we have RGBA or Alpha being passed in and the clip
	/// only supports the other one, so return that
	if( s == kOfxImageComponentRGBA && isSupportedComponent( kOfxImageComponentAlpha ) )
		return alpha;

	if( s == kOfxImageComponentAlpha && isSupportedComponent( kOfxImageComponentRGBA ) )
		return rgba;

	/// wierd, must be some custom bit , if only one, choose that, otherwise no idea
	/// how to map, you need to derive to do so.
	const std::vector<std::string>& supportedComps = getSupportedComponents();
	if( supportedComps.size() == 1 )
		return supportedComps[0];

	return none;
}

//////////////////////////////////////////////////////////////////////////////////
// ClipImageInstanceSet

OfxhClipImageSet::OfxhClipImageSet()
	: _clipPrefsDirty( true )
{}

OfxhClipImageSet::OfxhClipImageSet( const OfxhClipImageSet& other )
	: _clipsByOrder( other._clipsByOrder.clone() )
{
	initMapFromList();
}

void OfxhClipImageSet::initMapFromList()
{
	for( ClipImageVector::iterator it = _clipsByOrder.begin(), itEnd = _clipsByOrder.end();
	     it != itEnd;
	     ++it )
	{
		_clips[it->getName()] = &( *it );
	}
}

OfxhClipImageSet::~OfxhClipImageSet()
{}

bool OfxhClipImageSet::operator==( const This& other ) const
{
	return _clipsByOrder == other._clipsByOrder;
}

void OfxhClipImageSet::copyClipsValues( const OfxhClipImageSet& other )
{
	if( _clipsByOrder.size() != other._clipsByOrder.size() )
		throw core::exception::LogicError( "You try to copy clips values, but the two lists are not identical." );

	ClipImageVector::const_iterator oit = other._clipsByOrder.begin(), oitEnd = other._clipsByOrder.end();
	for( ClipImageVector::iterator it = _clipsByOrder.begin(), itEnd = _clipsByOrder.end();
	     it != itEnd && oit != oitEnd;
	     ++it, ++oit )
	{
		OfxhClipImage& c = *it;
		const OfxhClipImage& oc = *oit;
		if( c.getName() != oc.getName() )
			throw core::exception::LogicError( "You try to copy clips values, but it is not the same clips in the two lists." );
		c.copyValues(oc);
	}
}

void OfxhClipImageSet::populateClips( const imageEffect::OfxhImageEffectNodeDescriptor& descriptor ) OFX_EXCEPTION_SPEC
{
	const imageEffect::OfxhImageEffectNodeDescriptor::ClipImageDescriptorVector& clips = descriptor.getClipsByOrder();

	_clipsByOrder.reserve( clips.size() );
	/// @todo tuttle don't manipulate clip here, delegate to ClipInstanceSet
	for( imageEffect::OfxhImageEffectNodeDescriptor::ClipImageDescriptorVector::const_iterator it = clips.begin(), itEnd = clips.end();
	     it != itEnd;
	     ++it )
	{
		const std::string& name = it->getName();
		// foreach clip descriptor make a ClipImageInstance
		OfxhClipImage* instance = newClipImage( *it ); //( this, *it, counter );
		if( !instance )
			throw OfxhException( "Error on ClipImage creation." );

		_clipsByOrder.push_back( instance );
		_clips[name] = instance;
	}
}

}
}
}
}
