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

// ofx host
#include "OfxhBinary.hpp"
#include "OfxhPropertySuite.hpp"
#include "OfxhClip.hpp"
#include "OfxhImageEffect.hpp"

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {

////////////////////////////////////////////////////////////////////////////////
// props to clips descriptors and instances

// base ctor, for a descriptor
OfxhClipAccessor::OfxhClipAccessor()
{}

OfxhClipAccessor::~OfxhClipAccessor()
{}

/**
 * is the clip optional
 */
bool OfxhClipAccessor::isOptional() const
{
	return getProperties().getIntProperty( kOfxImageClipPropOptional ) != 0;
}

/**
 * descriptor
 */
OfxhClipDescriptor::OfxhClipDescriptor()
	: attribute::OfxhAttributeDescriptor()
{
	/// properties common to the desciptor and instance
	/// the desc and set them, the instance cannot
	static const property::OfxhPropSpec clipDescriptorStuffs[] = {
		{ kOfxPropType, property::eString, 1, true, kOfxTypeClip },
		{ kOfxImageClipPropOptional, property::eInt, 1, false, "0" },
		{ 0 },
	};

	getEditableProperties().addProperties( clipDescriptorStuffs );
}

/**
 * descriptor
 */
OfxhClipDescriptor::OfxhClipDescriptor( const property::OfxhSet& properties )
	: attribute::OfxhAttributeDescriptor( properties )
{
	/// properties common to the desciptor and instance
	/// the desc and set them, the instance cannot
	static const property::OfxhPropSpec clipDescriptorStuffs[] = {
		{ kOfxPropType, property::eString, 1, true, kOfxTypeClip },
		{ kOfxImageClipPropOptional, property::eInt, 1, false, "0" },
		{ 0 },
	};

	getEditableProperties().addProperties( clipDescriptorStuffs );
}

OfxhClipDescriptor::~OfxhClipDescriptor()
{}

////////////////////////////////////////////////////////////////////////////////
// instance

OfxhClip::OfxhClip( const OfxhClipDescriptor& desc )
	: attribute::OfxhAttribute( desc )
{
	/// extra properties for the instance, these are fetched from the host
	/// via a get hook and some virtuals
	static const property::OfxhPropSpec clipInstanceStuffs[] = {
		{ kOfxImageClipPropConnected, property::eInt, 1, true, "0" },
		{ 0 },
	};

	// properties that are needed in an Instance but not a Descriptor
	getEditableProperties().addProperties( clipInstanceStuffs );
	initHook( clipInstanceStuffs );

	/// we are an instance, we need to reset the props to read only
	for( property::PropertyMap::iterator i = getEditableProperties().getMap().begin(), iEnd = getEditableProperties().getMap().end();
			i != iEnd;
			++i )
	{
		i->second->setPluginReadOnly( false );
	}
}

OfxhClip::~OfxhClip()
{}

void OfxhClip::initHook( const property::OfxhPropSpec* propSpec )
{
	// no more GetHook on clip for the moment...
	/*
	 * int i = 0;
	 * while( propSpec[i].name )
	 * {
	 *  const Property::PropSpec& spec = propSpec[i];
	 *
	 *  switch( spec.type )
	 *  {
	 *      case Property::eDouble :
	 *      case Property::eString :
	 *      case Property::eInt :
	 *              getProperties().setGetHook( spec.name, this );
	 *          break;
	 *      default:
	 *          break;
	 *  }
	 ++i;
	 * }
	 */
}

}
}
}
}
