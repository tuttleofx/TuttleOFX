#include "NodeHashContainer.hpp"

#include <boost/foreach.hpp>

namespace tuttle
{
namespace host
{

std::ostream& operator<<(std::ostream& os, const NodeHashContainer& c)
{
    os << "abcd NodeHashContainer {" << std::endl;
    BOOST_FOREACH(const NodeHashContainer::Map::value_type& v, c._hashes)
    {
        os << v.first << ": " << v.second << std::endl;
    }
    os << "}" << std::endl;
    return os;
}
}
}
