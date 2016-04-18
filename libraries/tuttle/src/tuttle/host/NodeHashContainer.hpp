#ifndef _TUTTLE_HOST_NODEHASHCONTAINER_HPP_
#define _TUTTLE_HOST_NODEHASHCONTAINER_HPP_

#include "NodeAtTimeKey.hpp"
#include "exceptions.hpp"

#include <ofxCore.h>

#include <map>
#include <string>
#include <cstddef>
#include <ostream>

namespace tuttle
{
namespace host
{

class NodeHashContainer
{
public:
    NodeHashContainer() {}

    std::size_t getFirst() const { return _hashes.begin()->second; }
    std::size_t getHash(const NodeAtTimeKey& k) const
    {
        Map::const_iterator it = _hashes.find(k);
        if(it == _hashes.end())
        {
            BOOST_THROW_EXCEPTION(exception::BadIndex());
        }
        return it->second;
    }
    std::size_t getHash(const std::string& name, const OfxTime& time) const { return getHash(NodeAtTimeKey(name, time)); }

    void addHash(const std::string& name, const OfxTime& time, const std::size_t hash)
    {
        NodeAtTimeKey k(name, time);
        addHash(k, hash);
    }
    void addHash(const NodeAtTimeKey& k, const std::size_t hash) { _hashes[k] = hash; }

public:
    friend std::ostream& operator<<(std::ostream& os, const NodeHashContainer& c);

private:
    typedef std::map<NodeAtTimeKey, std::size_t> Map;
    Map _hashes;
};
}
}

#endif
