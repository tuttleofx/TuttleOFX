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

namespace attribute {

////////////////////////////////////////////////////////////////////////////////
// props to clips descriptors and instances

// base ctor, for a descriptor
ClipAccessor::ClipAccessor()
{}

ClipAccessor::ClipAccessor( const ClipAccessor& v )
	: attribute::AttributeAccessor( v )
{
	//TUTTLE_TODO : switch this function in private
	COUT_WITHINFOS( "This copy contructor should never be called." );
	assert( 0 );
}

ClipAccessor::~ClipAccessor()
{}

/** is the clip optional
 */
bool ClipAccessor::isOptional() const
{
	return getProperties().getIntProperty( kOfxImageClipPropOptional ) != 0;
}

/**
 * descriptor
 */
ClipDescriptor::ClipDescriptor()
	: attribute::AttributeDescriptor()
{
	/// properties common to the desciptor and instance
	/// the desc and set them, the instance cannot
	static const Property::PropSpec clipDescriptorStuffs[] = {
		{ kOfxPropType, Property::eString, 1, true, kOfxTypeClip },
		{ kOfxImageClipPropOptional, Property::eInt, 1, false, "0" },
		{ 0 },
	};

	getEditableProperties().addProperties( clipDescriptorStuffs );
}

/**
 * descriptor
 */
ClipDescriptor::ClipDescriptor( const Property::Set& properties )
	: attribute::AttributeDescriptor( properties )
{
	/// properties common to the desciptor and instance
	/// the desc and set them, the instance cannot
	static const Property::PropSpec clipDescriptorStuffs[] = {
		{ kOfxPropType, Property::eString, 1, true, kOfxTypeClip },
		{ kOfxImageClipPropOptional, Property::eInt, 1, false, "0" },
		{ 0 },
	};

	getEditableProperties().addProperties( clipDescriptorStuffs );
}

ClipDescriptor::~ClipDescriptor()
{}

////////////////////////////////////////////////////////////////////////////////
// instance

ClipInstance::ClipInstance( const ClipDescriptor& desc )
	: attribute::AttributeInstance( desc )
{
	/// extra properties for the instance, these are fetched from the host
	/// via a get hook and some virtuals
	static const Property::PropSpec clipInstanceStuffs[] = {
		{ kOfxImageClipPropConnected, Property::eInt, 1, true, "0" },
		{ 0 },
	};

	// properties that are needed in an Instance but not a Descriptor
	getEditableProperties().addProperties( clipInstanceStuffs );
	initHook( clipInstanceStuffs );

	/// we are an instance, we need to reset the props to read only
	const Property::PropertyMap& map = getProperties().getMap();
	Property::PropertyMap::const_iterator i;
	for( i = map.begin(); i != map.end(); ++i )
	{
		( *i ).second->setPluginReadOnly( false );
	}
}

ClipInstance::~ClipInstance()
{}

void ClipInstance::initHook( const Property::PropSpec* propSpec )
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
