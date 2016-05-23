#include "macos.hpp"

#ifdef __MACOS__

#include <string>

namespace tuttle
{
namespace common
{

std::string CFStringContainer::toString(CFStringRef cfString)
{
    CFIndex length = CFStringGetLength(cfString);
    CFIndex size = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8);
    char* buffer = new char[size];
    Boolean worked = CFStringGetCString(cfString, buffer, size, kCFStringEncodingUTF8);
    if(worked)
    {
        std::string result(buffer);
        delete[] buffer;
        return result;
    }
    delete[] buffer;
    return std::string();
}

CFStringContainer::operator std::string() const
{
    return str();
}

const std::string& CFStringContainer::str() const
{
    if(_string.empty() && _type)
        const_cast<CFStringContainer*>(this)->_string = toString(_type);
    return _string;
}

CFStringRef CFStringContainer::toCFStringRef(const std::string& s)
{
    return CFStringCreateWithCString(kCFAllocatorDefault, s.c_str(), kCFStringEncodingUTF8);
}

CFStringContainer::operator CFStringRef() const
{
    if(!_type)
    {
        const_cast<CFStringContainer*>(this)->_type = CFStringCreateWithCharactersNoCopy(
            0, reinterpret_cast<const UniChar*>(_string.c_str()), _string.size(), kCFAllocatorNull);
    }
    return _type;
}
}
}

#endif
