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

#include "OfxhHost.hpp"

#include "OfxhPropertySuite.hpp"
#include "OfxhMemorySuite.hpp"


// ofx
#include "ofxCore.h"
#include "ofxProperty.h"
#include "ofxMultiThread.h"
#include "ofxMemory.h"

#include <climits>
#include <cmath>
#include <cfloat>
#include <cstring>

//typedef OfxPlugin* ( *OfxGetPluginType )( int );
namespace tuttle {
namespace host {
namespace ofx {
/// our own internal property for storing away our private pointer to our host descriptor
#define kOfxHostSupportHostPointer "sf.openfx.net.OfxHostSupportHostPointer"

static property::OfxhPropSpec hostStuffs[] = {
	{ kOfxPropType, property::eString, 1, false, "Host" },
	{ kOfxPropName, property::eString, 1, false, "UNKNOWN" },
	{ kOfxPropLabel, property::eString, 1, false, "UNKNOWN" },
	{ kOfxHostSupportHostPointer,    property::ePointer,    0,    false,    NULL },
	{ 0 },
};

static void* fetchSuite( OfxPropertySetHandle hostProps, const char* suiteName, int suiteVersion )
{
	property::OfxhSet* properties = reinterpret_cast<property::OfxhSet*>( hostProps );

	OfxhAbstractHost* host = (OfxhAbstractHost*)properties->getPointerProperty( kOfxHostSupportHostPointer );

	if( host )
		return host->fetchSuite( suiteName, suiteVersion );
	else
		return 0;
}

// Base Host
OfxhAbstractHost::OfxhAbstractHost() : _properties( hostStuffs )
{
	_host.host       = _properties.getHandle();
	_host.fetchSuite = tuttle::host::ofx::fetchSuite;

	// record the host descriptor in the propert set
	_properties.setPointerProperty( kOfxHostSupportHostPointer, this );
}

OfxhAbstractHost::~OfxhAbstractHost() {}

OfxHost* OfxhAbstractHost::getHandle()
{
	return &_host;
}

void* OfxhAbstractHost::fetchSuite( const char* suiteName, const int suiteVersion )
{
	if( strcmp( suiteName, kOfxPropertySuite ) == 0  && suiteVersion == 1 )
	{
		return property::getPropertySuite( suiteVersion );
	}
	else if( strcmp( suiteName, kOfxMemorySuite ) == 0 && suiteVersion == 1 )
	{
		return getMemorySuite( suiteVersion );
	}

	///printf("fetchSuite failed with host = %p, name = %s, version = %i\n", this, suiteName, suiteVersion);
	return NULL;
}

}
}
}

