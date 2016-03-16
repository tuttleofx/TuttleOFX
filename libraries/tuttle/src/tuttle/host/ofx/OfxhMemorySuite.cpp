#include "OfxhMemorySuite.hpp"
#include "OfxhMemory.hpp"

#include "OfxhCore.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{

namespace
{

OfxStatus memoryAlloc(void* handle, size_t bytes, void** data)
{
    *data = new char[bytes];
    if(*data)
    {
        return kOfxStatOK;
    }
    else
    {
        return kOfxStatErrMemory;
    }
}

OfxStatus memoryFree(void* data)
{
    delete[] static_cast<char*>(data);
    return kOfxStatOK;
}

struct OfxMemorySuiteV1 gMallocSuite = {memoryAlloc, memoryFree};
}

void* getMemorySuite(const int version)
{
    if(version == 1)
        return &gMallocSuite;
    return NULL;
}
}
}
}
