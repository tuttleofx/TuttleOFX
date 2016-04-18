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
#ifndef _TUTTLE_HOST_OFX_HOST_HPP_
#define _TUTTLE_HOST_OFX_HOST_HPP_

#include "property/OfxhSet.hpp"

#include <ofxCore.h>
#include <ofxImageEffect.h>
#include <ofxTimeLine.h>

#include <string>

namespace tuttle
{
namespace host
{
namespace ofx
{

// a plugin what we use
class OfxhPlugin;

// a param descriptor
namespace attribute
{
class OfxhParamDescriptor;
}

/**
 * Base class for all objects passed to a plugin by the 'setHost' function
 * passed back by any plug-in.
 */
class OfxhHost
{
protected:
    OfxHost _host;
    property::OfxhSet _properties;

public:
    OfxhHost();
    virtual ~OfxhHost() = 0;

    /// get the props on this host
    const property::OfxhSet& getProperties() const { return _properties; }
    property::OfxhSet& getProperties() { return _properties; }

    /**
     * fetch a suite
     * The base class returns the following suites
     *    PropertySuite
     *    MemorySuite
     */
    virtual void* fetchSuite(const char* suiteName, const int suiteVersion);

    /// get the C API handle that is passed across the API to represent this host
    OfxHost* getHandle();

    /// is my magic number valid?
    bool verifyMagic() { return true; }

#ifndef SWIG

    /// override this to handle do post-construction initialisation on a Param::Descriptor
    virtual void initDescriptor(attribute::OfxhParamDescriptor&) const {}

    /// vmessage
    virtual OfxStatus vmessage(const char* type, const char* id, const char* format, va_list args) const = 0;
#endif
};
}
}
}

#endif
