#include "OfxhMessageSuite.hpp"
#include "OfxhMessage.hpp"
#include "OfxhCore.hpp"

#include <cstdarg>
#include <cstdio>

namespace tuttle
{
namespace host
{
namespace ofx
{

namespace
{

OfxStatus message(void* handle, const char* type, const char* id, const char* format, ...)
{
    OfxhIMessage* effectInstance = dynamic_cast<OfxhIMessage*>(reinterpret_cast<OfxhIObject*>(handle));

    if(effectInstance)
    {
        va_list args;
        va_start(args, format);
        effectInstance->vmessage(type, id, format, args);
        va_end(args);
    }
    else
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
    return kOfxStatOK;
}

/// message suite for an image effect plugin
struct OfxMessageSuiteV1 gMessageSuite = {message};
}

void* getMessageSuite(const int version)
{
    if(version == 1)
        return static_cast<void*>(&gMessageSuite);
    return NULL;
}
}
}
}
