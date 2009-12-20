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

#include "ofxImageEffect.h"
#include "OfxhAttribute.hpp"
#include "OfxhUtilities.hpp"
#include "OfxhPropertySuite.hpp"

#include <boost/utility.hpp>

namespace tuttle {
namespace host {
namespace ofx {

namespace imageEffect {
class OfxhImageEffectNode;
}

namespace attribute {

/**
 * Base to both descriptor and instance it
 * is used to basically fetch common properties
 * by function name
 */
class OfxhClipAccessor : virtual public attribute::OfxhAttributeAccessor
{
public:
	/// @brief base ctor, for a descriptor
	OfxhClipAccessor();

	virtual ~OfxhClipAccessor() = 0;

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
class OfxhClipDescriptor : virtual public OfxhClipAccessor,
	public attribute::OfxhAttributeDescriptor
{
public:
	/// constructor
	OfxhClipDescriptor();
	OfxhClipDescriptor( const tuttle::host::ofx::property::OfxhSet& );
	~OfxhClipDescriptor() = 0;
};

/**
 * a clip instance
 */
class OfxhClip : virtual public OfxhClipAccessor,
	public attribute::OfxhAttribute,
	protected property::OfxhGetHook,
	protected property::OfxhNotifyHook,
	private boost::noncopyable
{
protected:
	OfxhClip( const OfxhClip& other ) : attribute::OfxhAttribute( other ) {}

public:
	OfxhClip( const OfxhClipDescriptor& desc );
	virtual ~OfxhClip() = 0;

	/// clone this clip
	virtual OfxhClip* clone() const = 0;

	void initHook( const property::OfxhPropSpec* propSpec );

	/// notify override properties

	void notify( const std::string& name, bool isSingle, int indexOrN ) OFX_EXCEPTION_SPEC
	{
		COUT_WITHINFOS( "What we should do here?" );
		throw property::OfxhException( kOfxStatErrMissingHostFeature );
	}

	// don't know what to do

	void reset( const std::string& name ) OFX_EXCEPTION_SPEC
	{
		COUT_WITHINFOS( "What we should do here?" );
		throw property::OfxhException( kOfxStatErrMissingHostFeature );
	}

	// Connected -
	//
	//  Says whether the clip is actually connected at the moment.
	virtual const bool getConnected() const = 0;
};

/**
 * @brief to make ClipInstance clonable (for use in boost::ptr_container)
 */
inline OfxhClip* new_clone( const OfxhClip& a )
{
	return a.clone();
}

}
}
}
}

#endif
