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

#ifndef OFXH_CLIP_H
#define OFXH_CLIP_H

#include "ofxhAttribute.h"
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
 * Base to both descriptor and instance it
 * is used to basically fetch common properties
 * by function name
 */
class ClipAccessor : virtual public attribute::AttributeAccessor
{
public:
	/// @brief base ctor, for a descriptor
	ClipAccessor();

	/// @brief ctor, when copy constructing an instance from a descriptor
	explicit ClipAccessor( const ClipAccessor& other );

	virtual ~ClipAccessor() = 0;

	/// is the clip optional
	bool isOptional() const;

	virtual bool verifyMagic()
	{
		return true;
	}

};

/**
 * a clip descriptor
 */
class ClipDescriptor : virtual public ClipAccessor,
	public attribute::AttributeDescriptor
{
public:
	/// constructor
	ClipDescriptor();
	ClipDescriptor( const tuttle::host::ofx::Property::Set& );
	~ClipDescriptor() = 0;
};

/**
 * a clip instance
 */
class ClipInstance : virtual public ClipAccessor,
	public attribute::AttributeInstance,
	protected Property::GetHook,
	protected Property::NotifyHook
{
public:
	ClipInstance( const ClipDescriptor& desc );
	virtual ~ClipInstance() = 0;

	void initHook( const Property::PropSpec* propSpec );

	/// notify override properties

	void notify( const std::string& name, bool isSingle, int indexOrN ) OFX_EXCEPTION_SPEC
	{
		COUT_WITHINFOS( "What we should do here?" );
		throw Property::Exception( kOfxStatErrMissingHostFeature );
	}

	// don't know what to do

	void reset( const std::string& name ) OFX_EXCEPTION_SPEC
	{
		COUT_WITHINFOS( "What we should do here?" );
		throw Property::Exception( kOfxStatErrMissingHostFeature );
	}

	// Connected -
	//
	//  Says whether the clip is actually connected at the moment.
	virtual bool getConnected() const = 0;
};

}
}
}
}

#endif
