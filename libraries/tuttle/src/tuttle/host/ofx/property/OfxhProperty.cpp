/*
 * Software License :
 *
 * Copyright (c) 2007-2009, The Open Effects Association Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
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
#include "OfxhProperty.hpp"
#include "OfxhGetHook.hpp"
#include "OfxhNotifyHook.hpp"

#include <tuttle/host/ofx/OfxhCore.hpp>
#include <tuttle/host/exceptionsHost.hpp>
#include <tuttle/host/serialization.hpp>

// ofx
#include <ofxImageEffect.h>

#include <iostream>
#include <cstring>

//#define DEBUG_PROPERTIES true

namespace tuttle {
namespace host {
namespace ofx {
namespace property {

OfxhProperty::OfxhProperty( const std::string& name,
                            TypeEnum           type,
                            size_t             dimension,
                            bool               pluginReadOnly )
	: _name( name )
	, _type( type )
	, _dimension( dimension )
	, _pluginReadOnly( pluginReadOnly )
	, _modifiedBy( eModifiedByHost )
	, _getHook( NULL )
{}

OfxhProperty::OfxhProperty( const OfxhProperty& other )
	: _name( other._name )
	, _type( other._type )
	, _dimension( other._dimension )
	, _pluginReadOnly( other._pluginReadOnly )
	, _modifiedBy( other._modifiedBy )
	, _getHook( NULL )
{}

OfxhProperty::~OfxhProperty()
{}

/// call notify on the contained notify hooks
void OfxhProperty::notify( bool single, int indexOrN )
{
	std::vector<OfxhNotifyHook*>::iterator i;
	for( i = _notifyHooks.begin(); i != _notifyHooks.end(); ++i )
	{
		( *i )->notify( _name, single, indexOrN );
	}
}

}
}
}
}
