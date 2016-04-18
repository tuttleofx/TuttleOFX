#ifndef _TUTTLE_HOST_PROCESSVERTEXATTIME_HPP_
#define _TUTTLE_HOST_PROCESSVERTEXATTIME_HPP_

#include "IVertex.hpp"
#include "ProcessVertex.hpp"
#include "ProcessVertexData.hpp"
#include "ProcessVertexAtTimeData.hpp"

#include <tuttle/host/NodeAtTimeKey.hpp>

#include <ostream>
#include <string>

namespace tuttle
{
namespace host
{
namespace graph
{

class ProcessVertexAtTime : public IVertex
{
public:
    typedef NodeAtTimeKey Key;

public:
    ProcessVertexAtTime();
    ProcessVertexAtTime(const ProcessVertex& v, const OfxTime t);
    ProcessVertexAtTime(const ProcessVertexAtTime& other);
    virtual ~ProcessVertexAtTime();

    ProcessVertexAtTime& operator=(const ProcessVertexAtTime& v)
    {
        IVertex::operator=(v);
        _data = v._data;
        return *this;
    }

    Key getKey() const { return Key(_clipName, _data._time); }

    const ProcessVertexData& getProcessData() const { return *_data._nodeData; }
    ProcessVertexAtTimeData& getProcessDataAtTime() { return _data; }
    const ProcessVertexAtTimeData& getProcessDataAtTime() const { return _data; }

    std::ostream& exportDotDebug(std::ostream& os) const;
    friend std::ostream& operator<<(std::ostream& os, const ProcessVertexAtTime& v);
    friend std::ostream& operator<<(std::ostream& os, const Key& v);

public:
    std::string _clipName;
    ProcessVertexAtTimeData _data;
};

std::ostream& operator<<(std::ostream& os, const ProcessVertexAtTime& v);
}
}
}

namespace boost
{
inline std::size_t hash_value(const tuttle::host::graph::ProcessVertexAtTime::Key& k)
{
    return k.getHash();
}
}

#endif
