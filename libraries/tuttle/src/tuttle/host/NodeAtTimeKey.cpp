#include "NodeAtTimeKey.hpp"

#include <boost/functional/hash.hpp>

namespace tuttle
{
namespace host
{

bool NodeAtTimeKey::operator==(const NodeAtTimeKey& v) const
{
    return (v._name == _name) && (v._time == _time);
}

bool NodeAtTimeKey::operator<(const NodeAtTimeKey& v) const
{
    if(v._name == _name)
    {
        return v._time < _time;
    }
    return v._name < _name;
}

std::size_t NodeAtTimeKey::getHash() const
{
    std::size_t seed = 0;
    boost::hash_combine(seed, _name);
    boost::hash_combine(seed, _time);
    return seed;
}

std::ostream& operator<<(std::ostream& os, const NodeAtTimeKey& k)
{
    os << "(" << k._name << ", " << k._time << ")";
    return os;
}
}
}
