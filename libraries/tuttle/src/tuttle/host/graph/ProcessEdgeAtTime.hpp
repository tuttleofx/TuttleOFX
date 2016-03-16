#ifndef _TUTTLE_HOST_PROCESSEDGEATTIME_HPP_
#define _TUTTLE_HOST_PROCESSEDGEATTIME_HPP_

#include "IEdge.hpp"
#include "ProcessEdge.hpp"
#include "ProcessVertexAtTime.hpp"

namespace tuttle
{
namespace host
{
namespace graph
{

class ProcessEdgeAtTime : public IEdge
{
public:
    ProcessEdgeAtTime();
    ProcessEdgeAtTime(const ProcessVertexAtTime::Key& out, const ProcessVertexAtTime::Key& in,
                      const std::string& inAttrName);
    ProcessEdgeAtTime(const ProcessEdge& e);
    ProcessEdgeAtTime(const ProcessEdgeAtTime& other);
    virtual ~ProcessEdgeAtTime();

    std::ostream& exportDotDebug(std::ostream& os) const;

    inline ProcessEdgeAtTime& operator=(const ProcessEdge& e)
    {
        IEdge::operator=(e);
        return *this;
    }

    inline OfxTime getOutTime() const { return _outTime; }
    inline OfxTime getInTime() const { return _inTime; }

private:
    OfxTime _inTime;
    OfxTime _outTime;
};
}
}
}

#endif
