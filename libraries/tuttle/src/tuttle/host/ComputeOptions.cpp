#include "ComputeOptions.hpp"

#include <boost/lexical_cast.hpp>

namespace tuttle
{
namespace host
{

IProgressHandle::~IProgressHandle()
{
}

TimeRange::TimeRange(const OfxRangeD& range, const int step)
    : _begin(static_cast<int>(range.min))
    , _end(static_cast<int>(range.max))
    , _step(step)
{
}
}
}
