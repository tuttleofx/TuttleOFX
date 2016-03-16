#ifndef _TUTTLE_COMMON_SYSTEM_MACOS_HPP_
#define _TUTTLE_COMMON_SYSTEM_MACOS_HPP_

#include "system.hpp"

#ifdef __MACOS__

//#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#undef nil // /usr/include/MacTypes.h defines nil as NULL. This borks boost.

#include <string>

namespace tuttle
{
namespace common
{

/*
        Helper class that automates refernce counting for CFtypes.
        After constructing the CFTypeContainer object, it can be copied like a
        value-based type.

        Note that you must own the object you are wrapping.
        This is typically the case if you get the object from a Core
        Foundation function with the word "Create" or "Copy" in it. If
        you got the object from a "Get" function, either retain it or use
        constructFromGet(). One exception to this rule is the
        HIThemeGet*Shape functions, which in reality are "Copy" functions.
 */
template <typename T>
class CFTypeContainer
{
public:
    inline CFTypeContainer(const T& t = 0)
        : _type(t)
    {
    }

    inline CFTypeContainer(const CFTypeContainer& helper)
        : _type(helper._type)
    {
        if(_type)
            CFRetain(_type);
    }

    inline ~CFTypeContainer()
    {
        if(_type)
            CFRelease(_type);
    }

    inline operator T() { return _type; }

    inline CFTypeContainer operator=(const CFTypeContainer& helper)
    {
        if(helper._type)
            CFRetain(helper._type);
        CFTypeRef type2 = _type;
        _type = helper._type;
        if(type2)
            CFRelease(type2);
        return *this;
    }

    inline T* operator&() { return &_type; }

    static CFTypeContainer constructFromGet(const T& t)
    {
        CFRetain(t);
        return CFTypeContainer<T>(t);
    }

protected:
    T _type;
};

class CFStringContainer : public CFTypeContainer<CFStringRef>
{
public:
    inline CFStringContainer(const std::string& str)
        : CFTypeContainer<CFStringRef>(0)
        , _string(str)
    {
    }

    inline CFStringContainer(const CFStringRef cfstr = NULL)
        : CFTypeContainer<CFStringRef>(cfstr)
    {
    }

    inline CFStringContainer(const CFTypeContainer<CFStringRef>& other)
        : CFTypeContainer<CFStringRef>(other)
    {
    }
    operator std::string() const;
    operator CFStringRef() const;

    const std::string& str() const;

    static std::string toString(CFStringRef cfstr);
    static CFStringRef toCFStringRef(const std::string& str);

private:
    std::string _string;
};
}
}

#endif

#endif
