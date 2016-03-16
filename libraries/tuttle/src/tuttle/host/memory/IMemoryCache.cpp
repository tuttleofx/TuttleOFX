#include "IMemoryCache.hpp"

#include <boost/functional/hash.hpp>
#include <ostream>

namespace tuttle
{
namespace host
{
namespace memory
{

bool Key::operator<(const Key& other) const
{
    if(_time != other._time)
        return _time < other._time;
    return _identifier < other._identifier;
}

bool Key::operator==(const Key& v) const
{
    return _time == v._time && _identifier == v._identifier;
}

std::size_t Key::getHash() const
{
    std::size_t seed = 0;

    boost::hash_combine(seed, _time);
    boost::hash_combine(seed, _identifier);
    return seed;
}

std::ostream& operator<<(std::ostream& os, const Key& v)
{
    os << "[identifier:" << v._identifier << ", time:" << v._time << "]";
    return os;
}

IMemoryCache::~IMemoryCache()
{
}

std::ostream& operator<<(std::ostream& os, const IMemoryCache& v)
{
    return v.outputStream(os);
}
}
}
}
