/*
Software License :

Copyright (c) 2007-2009, The Open Effects Association Ltd. All rights reserved.

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

// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhParam.h"
#include "ofxhImageEffect.h"

#include <assert.h>
#include <float.h>
#include <limits.h>
#include <stdarg.h>

namespace tuttle {
namespace host {
namespace ofx {
namespace attribute {
			
			AttributeAccessor::AttributeAccessor()
			{
			}

			AttributeAccessor::~AttributeAccessor( ) { }

                        AttributeDescriptor::AttributeDescriptor( )
                        : _properties(Property::Set())
                        {
                                /// properties common to the desciptor and instance
                                /// the desc and set them, the instance cannot
                                static Property::PropSpec attributeDescriptorStuffs[] = {
                                        { kOfxPropName, Property::eString, 1, true, "SET ME ON CONSTRUCTION" },
                                        { kOfxPropLabel, Property::eString, 1, false, "" },
                                        { kOfxPropShortLabel, Property::eString, 1, false, "" },
                                        { kOfxPropLongLabel, Property::eString, 1, false, "" },
                                        { 0 },
                                };
                                getEditableProperties().addProperties( attributeDescriptorStuffs );
                        }

			AttributeDescriptor::AttributeDescriptor( const Property::Set& properties )
			:_properties(properties)
			{
				/// properties common to the desciptor and instance
				/// the desc and set them, the instance cannot
				static Property::PropSpec attributeDescriptorStuffs[] = {
					{ kOfxPropName, Property::eString, 1, true, "SET ME ON CONSTRUCTION" },
					{ kOfxPropLabel, Property::eString, 1, false, "" },
					{ kOfxPropShortLabel, Property::eString, 1, false, "" },
					{ kOfxPropLongLabel, Property::eString, 1, false, "" },
					{ 0 },
				};
				getEditableProperties().addProperties( attributeDescriptorStuffs );
			}

			AttributeDescriptor::~AttributeDescriptor(){}

			AttributeInstance::AttributeInstance( const Property::Set& properties )
			:_properties(properties)
			{
			}

			AttributeInstance::AttributeInstance( const AttributeDescriptor& desc )
			: _properties(desc.getProperties())
			{
			}

			AttributeInstance::~AttributeInstance(){}

}
}
}
}
