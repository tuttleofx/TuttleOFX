#include "pointerParam.hpp"

#include <boost/lexical_cast.hpp>

#include <string>

namespace tuttle
{
namespace plugin
{
namespace memoryBuffer
{

void* stringToPointer(const std::string& value)
{
    if(value.empty())
        return NULL;
    std::ptrdiff_t p = boost::lexical_cast<std::ptrdiff_t>(value);
    return reinterpret_cast<void*>(p);
}
}
}
}
