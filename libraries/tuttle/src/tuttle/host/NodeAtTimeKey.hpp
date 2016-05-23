#ifndef _TUTTLE_HOST_NODEATTIMEKEY_HPP_
#define _TUTTLE_HOST_NODEATTIMEKEY_HPP_

#include <ofxCore.h>

#include <string>
#include <ostream>

namespace tuttle
{
namespace host
{

class NodeAtTimeKey
{
public:
    NodeAtTimeKey()
        : _time(-9876.54321)
    {
    }
    NodeAtTimeKey(const std::string& name, const OfxTime& time)
        : _name(name)
        , _time(time)
    {
    }

    const std::string& getName() const { return _name; }
    void setName(const std::string& name) { _name = name; }

    OfxTime getTime() const { return _time; }
    void setTime(const OfxTime time) { _time = time; }

    bool operator==(const NodeAtTimeKey& v) const;
    bool operator<(const NodeAtTimeKey& v) const;

    std::size_t getHash() const;

public:
    friend std::ostream& operator<<(std::ostream& os, const NodeAtTimeKey& k);

private:
    std::string _name;
    OfxTime _time;
};
}
}

#endif
