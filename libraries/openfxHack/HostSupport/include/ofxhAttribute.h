
/*
Software License :

Copyright (c) 2007-2009, The Open Effects Association Ltd.  All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
	  this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.
 * Neither the name The Open Effects Association Ltd, nor the names of its
	  contributors may be used to endorse or promote products derived from this
	  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OFXH_ATTRIBUTE_H
#define OFXH_ATTRIBUTE_H
#include "ofxhPropertySuite.h"
#include "ofxCore.h"

#include <string>
#include <map>
#include <list>
#include <cstdarg>
#include <cassert>

namespace OFX {

namespace Host {

class Attribute
{
public:
	Attribute( const std::string &name, const std::string &type );
	Attribute( const std::string &name, const std::string &type, const Property::Set &properties );
	virtual ~Attribute( );

protected:
	std::string _name;
	std::string _type;
	Property::Set _properties;
	
public:
	const Property::Set &getProperties( ) const;
	Property::Set &getProperties( );

};

}
}

#endif // OFXH_PARAM_H
