#ifndef _TUTTLE_HOST_OFX_MESSAGE_HPP_
#define _TUTTLE_HOST_OFX_MESSAGE_HPP_

#include "OfxhException.hpp"
#include "OfxhIObject.hpp"
#include <cstdarg>
#include <string>

namespace tuttle
{
namespace host
{
namespace ofx
{

class OfxhIMessage : virtual public OfxhIObject
{
public:
    virtual ~OfxhIMessage() {}
#ifndef SWIG
    virtual void vmessage(const char* type, const char* id, const char* format, va_list args) const OFX_EXCEPTION_SPEC = 0;
#endif
};
}
}
}

#endif
