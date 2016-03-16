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
#ifndef OFXH_BINARY_H
#define OFXH_BINARY_H

#include "OfxhCore.hpp"

#include <boost/serialization/export.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include <string>
#include <iostream>

#ifndef WINDOWS
#if(defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64) || defined(__WINDOWS__) ||                       \
    defined(__TOS_WIN__) || defined(__WIN32__))
#define WINDOWS
#endif
#endif

#if defined(_WIN32) || defined(_WIN64)
#define I386
#elif defined(__linux__)
#ifndef UNIX
#define UNIX
#endif
#ifdef __i386__
#define I386
#elif defined(__amd64__)
#define AMD64
#else
#error cannot detect architecture
#endif
#elif defined(__APPLE__)
#ifndef UNIX
#define UNIX
#endif
#else
#error cannot detect operating system
#endif

#if defined(UNIX)
#include <dlfcn.h>
#elif defined(WINDOWS)
#include "windows.h"
#include <cassert>
#endif

#include <sys/stat.h>

namespace tuttle
{
namespace host
{
namespace ofx
{

/// class representing a DLL/Shared Object/etc
class OfxhBinary
{
public:
    typedef OfxhBinary This;
    /// destruction will close the library and invalidate
    /// any function pointers returned by lookupSymbol()

protected:
    std::string _binaryPath;
    bool _invalid;
#if defined(UNIX)
    void* _dlHandle;
#elif defined(WINDOWS)
    HINSTANCE _dlHandle;
#endif
    bool _exists;
    time_t _time;
    size_t _size;
    int _users;

public:
    OfxhBinary();

    void init(const std::string& binaryPath);

    /// create object representing the binary.  will stat() it,
    /// and this fails, will set binary to be invalid.
    OfxhBinary(const std::string& binaryPath);

    ~OfxhBinary() { unload(); }

    bool operator==(const This& other) const
    {
        if(_binaryPath != other._binaryPath || _invalid != other._invalid || _exists != other._exists ||
           _time != other._time || _size != other._size)
            return false;
        return true;
    }

    bool operator!=(const This& other) const { return !This::operator==(other); }

    bool isLoaded() const { return _dlHandle != 0; }

    /// is this binary invalid? (did the a stat() or load() on the file fail,
    /// or are we missing a some of the symbols?
    bool isInvalid() const { return _invalid; }

    /// set invalid status (e.g. called by user if a mandatory symbol was missing)
    void setInvalid(bool invalid) { _invalid = invalid; }

    /// Last modification time of the file.
    time_t getTime() const { return _time; }

    /// Current size of the file.
    size_t getSize() const { return _size; }

    /// Path to the file.
    const std::string& getBinaryPath() const { return _binaryPath; }

    void ref();
    void unref();

    /// open the binary.
    void load();

    /// close the binary
    void unload();

    /// look up a symbol in the binary file and return it as a pointer.
    /// returns null pointer if not found, or if the library is not loaded.
    void* findSymbol(const std::string& symbol);

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& BOOST_SERIALIZATION_NVP(_binaryPath);
        ar& BOOST_SERIALIZATION_NVP(_invalid);
        ar& BOOST_SERIALIZATION_NVP(_exists);
        ar& BOOST_SERIALIZATION_NVP(_size);
        ar& BOOST_SERIALIZATION_NVP(_time);

        //		ar & BOOST_SERIALIZATION_NVP(_users);

        if(typename Archive::is_loading())
        {
            _users = 0;
            init(_binaryPath);
        }
    }
};
}
}
}

#endif
