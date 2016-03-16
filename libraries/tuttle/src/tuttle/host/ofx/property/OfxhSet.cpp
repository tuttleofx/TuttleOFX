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
#include "OfxhSet.hpp"
#include "OfxhPropertyTemplate.hpp"
#include "OfxhGetHook.hpp"
#include "OfxhNotifyHook.hpp"

#include <tuttle/host/ofx/OfxhCore.hpp>

#include <ofxCore.h>
#include <ofxImageEffect.h>

#include <iostream>
#include <cstring>

//#define DEBUG_PROPERTIES true

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace property
{

OfxhProperty& OfxhSet::localAt(const int index)
{
    if(index >= getLocalSize())
    {
        if(index >= getSize())
        {
            BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrValue)
                                  << exception::dev() + "OfxhSet::at: " + index + ", property not found.");
        }
        else
        {
            BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrValue)
                                  << exception::dev() + "OfxhSet::at: " + index + " is a non-local property.");
        }
    }

    PropertyMap::iterator it = _props.begin();
    std::advance(it, index);
    return *it->second;
}

const OfxhProperty& OfxhSet::at(const int index) const
{
    if(index < getLocalSize())
    {
        PropertyMap::const_iterator it = _props.begin();
        std::advance(it, index);
        return *it->second;
    }
    if(_chainedSet == NULL)
    {
        BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrValue)
                              << exception::dev() + "OfxhSet::at: " + index + ", property not found.");
    }
    return _chainedSet->at(index - getLocalSize());
}

void OfxhSet::setGetHook(const std::string& s, OfxhGetHook* ghook)
{
    fetchLocalProperty(s).setGetHook(ghook);
}

/**
 * add a notify hook for a particular property.  users may need to call particular
 * specialised versions of this.
 */
void OfxhSet::addNotifyHook(const std::string& s, OfxhNotifyHook* hook)
{
    fetchLocalProperty(s).addNotifyHook(hook);
}

OfxhProperty& OfxhSet::fetchLocalProperty(const std::string& name)
{
    PropertyMap::iterator i = _props.find(name);

    if(i == _props.end())
    {
        BOOST_THROW_EXCEPTION(OfxhException(
            kOfxStatErrValue, "fetchLocalProperty: " + name + ". Property not found.")); //+ " on type:" +
                                                                                         //getStringProperty(kOfxPropType) +
                                                                                         //" name:" +
                                                                                         //getStringProperty(kOfxPropName)
                                                                                         //);// " NULL, (followChain: " <<
                                                                                         //followChain << ").";
    }
    return *(i->second);
}

const OfxhProperty& OfxhSet::fetchProperty(const std::string& name) const
{
    PropertyMap::const_iterator i = _props.find(name);

    if(i == _props.end())
    {
        if(_chainedSet)
        {
            return _chainedSet->fetchProperty(name);
        }
        BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrValue)
                              << exception::dev() + "fetchProperty: " + name + " property not found.");
    }
    return *(i->second);
}

/**
 * add one new property
 */
void OfxhSet::createProperty(const OfxhPropSpec& spec)
{
    if(_props.find(spec.name) != _props.end())
    {
        BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrExists)
                              << exception::dev() + "Tried to add a duplicate property to a Property::Set (" + spec.name +
                                     ")");
    }
    std::string key(spec.name); // for constness
    switch(spec.type)
    {
        case ePropTypeInt:
            _props.insert(key, new Int(spec.name, spec.dimension, spec.readonly,
                                       spec.defaultValue ? std::atoi(spec.defaultValue) : 0));
            break;
        case ePropTypeDouble:
            _props.insert(key, new Double(spec.name, spec.dimension, spec.readonly,
                                          spec.defaultValue ? std::atof(spec.defaultValue) : 0));
            break;
        case ePropTypeString:
            _props.insert(key,
                          new String(spec.name, spec.dimension, spec.readonly, spec.defaultValue ? spec.defaultValue : ""));
            break;
        case ePropTypePointer:
            _props.insert(key, new Pointer(spec.name, spec.dimension, spec.readonly, (void*)spec.defaultValue));
            break;
        case ePropTypeNone:
            BOOST_THROW_EXCEPTION(OfxhException(kOfxStatErrUnsupported)
                                  << exception::dev() + "Tried to create a property of an unrecognized type (" + spec.name +
                                         ", " + mapTypeEnumToString(spec.type) + ")");
    }
}

void OfxhSet::addProperties(const OfxhPropSpec spec[])
{
    while(spec->name)
    {
        createProperty(*spec);
        ++spec;
    }
}

void OfxhSet::eraseProperty(const std::string& propName)
{
    _props.erase(propName);
}

bool OfxhSet::hasProperty(const std::string& propName, bool followChain) const
{
    PropertyMap::const_iterator it = _props.find(propName);

    if(it == _props.end())
    {
        if(followChain && _chainedSet)
        {
            return _chainedSet->hasProperty(propName, true);
        }
    }
    return it != _props.end();
}

bool OfxhSet::hasLocalProperty(const std::string& propName) const
{
    return hasProperty(propName, false);
}

/**
 * add one new property
 */
void OfxhSet::addProperty(OfxhProperty* prop)
{
    std::string key(prop->getName()); // for constness

    _props.insert(key, prop);
}

/**
 * empty ctor
 */
OfxhSet::OfxhSet()
    : _magic(kMagic)
    , _chainedSet(NULL)
{
}

OfxhSet::OfxhSet(const OfxhPropSpec spec[])
    : _magic(kMagic)
    , _chainedSet(NULL)
{
    addProperties(spec);
}

OfxhSet::OfxhSet(const OfxhSet& other)
    : _magic(kMagic)
{
    operator=(other);
}

OfxhSet::~OfxhSet()
{
    clear();
}

void OfxhSet::clear()
{
    _props.clear();
}

OfxhSet& OfxhSet::operator=(const This& other)
{
    _props = other._props.clone();
    _chainedSet = other._chainedSet;
    return *this;
}

bool OfxhSet::operator==(const This& other) const
{
    if(_props != other._props)
        return false;
    if(_chainedSet == NULL)
    {
        if(other._chainedSet != NULL)
            if(other._chainedSet->getSize() != 0)
                return false;
    }
    else
    {
        if(other._chainedSet == NULL)
            if(_chainedSet->getSize() != 0)
                return false;
        if(*_chainedSet != *(other._chainedSet))
            return false;
    }
    return true;
}

void OfxhSet::copyValues(const This& other)
{
    if(_props.size() != other._props.size())
    {
        BOOST_THROW_EXCEPTION(exception::Bug()
                              << exception::dev("You try to copy properties values, but the two lists are not identical."));
    }

    PropertyMap::const_iterator oit = other._props.begin(), oitEnd = other._props.end();
    for(PropertyMap::iterator it = _props.begin(), itEnd = _props.end(); it != itEnd && oit != oitEnd; ++it, ++oit)
    {
        OfxhProperty& p = *(it->second);
        const OfxhProperty& op = *(oit->second);
        if(p.getName() != op.getName())
        {
            BOOST_THROW_EXCEPTION(
                exception::Bug() << exception::dev(
                    "You try to copy properties values, but it is not the same property in the two lists."));
        }
        p.copyValues(op);
    }
}

/// get a particular int property
int OfxhSet::getIntPropertyRaw(const std::string& property, int index) const
{
    return getPropertyRaw<OfxhIntValue>(property, index);
}

/// get a particular double property

double OfxhSet::getDoublePropertyRaw(const std::string& property, int index) const
{
    return getPropertyRaw<OfxhDoubleValue>(property, index);
}

/// get a particular double property

void* OfxhSet::getPointerPropertyRaw(const std::string& property, int index) const
{
    return getPropertyRaw<OfxhPointerValue>(property, index);
}

/// get a particular double property

const std::string& OfxhSet::getStringPropertyRaw(const std::string& property, int index) const
{
    return fetchTypedProperty<String>(property).getValueRaw(index);
    // return OfxhStringValue::kEmpty;
}

/// get a particular int property

int OfxhSet::getIntProperty(const std::string& property, int index) const
{
    return getProperty<OfxhIntValue>(property, index);
}

/// get the value of a particular double property

void OfxhSet::getIntPropertyN(const std::string& property, int* v, int N) const
{
    return getPropertyN<OfxhIntValue>(property, N, v);
}

/// get a particular double property

double OfxhSet::getDoubleProperty(const std::string& property, int index) const
{
    return getProperty<OfxhDoubleValue>(property, index);
}

/// get the value of a particular double property

void OfxhSet::getDoublePropertyN(const std::string& property, double* v, int N) const
{
    return getPropertyN<OfxhDoubleValue>(property, N, v);
}

/// get a particular double property

void* OfxhSet::getPointerProperty(const std::string& property, int index) const
{
    return getProperty<OfxhPointerValue>(property, index);
}

/// get a particular double property

const std::string& OfxhSet::getStringProperty(const std::string& property, int index) const
{
    return getProperty<OfxhStringValue>(property, index);
}

/// is the given string one of the values of a multi-dimensional string prop
/// this returns a non negative index if it is found, otherwise, -1

int OfxhSet::findStringPropValueIndex(const std::string& propName, const std::string& propValue) const
{
    const String& prop = fetchStringProperty(propName);

    const std::vector<std::string>& values = prop.getValues();
    std::vector<std::string>::const_iterator i = find(values.begin(), values.end(), propValue);
    if(i != values.end())
    {
        return int(i - values.begin());
    }
    return -1;
}

std::ostream& operator<<(std::ostream& os, const OfxhSet& v)
{
    os << "property::Set {" << std::endl;
    for(PropertyMap::const_iterator it = v._props.begin(), itEnd = v._props.end(); it != itEnd; ++it)
    {
        const OfxhProperty& prop = *(it->second);
        os << "    " << it->first << " ";
        os << "(type:" << mapTypeEnumToString(prop.getType()) << " dim:" << prop.getDimension()
           << " ro:" << prop.getPluginReadOnly()
           << " modifiedBy:" << (prop.getModifiedBy() == eModifiedByHost ? "host" : "plugin") << ") : [";
        int i = 0;
        for(; i < (int)(prop.getDimension()) - 1; ++i)
        {
            os << prop.getStringValueAt(i) << ", ";
        }
        if(prop.getDimension() > 0)
            os << prop.getStringValueAt(i);
        os << "] " << std::endl;
    }
    os << "}" << std::endl;
    return os;
}
}
}
}
}
