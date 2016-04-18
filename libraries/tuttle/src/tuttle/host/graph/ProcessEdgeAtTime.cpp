#include "ProcessEdgeAtTime.hpp"
#include "GraphExporter.hpp"

namespace tuttle
{
namespace host
{
namespace graph
{

ProcessEdgeAtTime::ProcessEdgeAtTime()
    : _inTime(0.0)
    , _outTime(0.0)
{
}

ProcessEdgeAtTime::ProcessEdgeAtTime(const ProcessVertexAtTime::Key& out, const ProcessVertexAtTime::Key& in,
                                     const std::string& inAttrName)
    : IEdge(out.getName(), in.getName(), inAttrName)
    , _inTime(in.getTime())
    , _outTime(out.getTime())
{
}

ProcessEdgeAtTime::ProcessEdgeAtTime(const ProcessEdge& e)
    : IEdge(e)
    , _inTime(0.0)
    , _outTime(0.0)
{
}

ProcessEdgeAtTime::ProcessEdgeAtTime(const ProcessEdgeAtTime& other)
    : IEdge(other)
    , _inTime(other._inTime)
    , _outTime(other._outTime)
{
}

ProcessEdgeAtTime::~ProcessEdgeAtTime()
{
}

std::ostream& ProcessEdgeAtTime::exportDotDebug(std::ostream& os) const
{
    std::ostringstream s;
    s << subDotEntry("label", getName());
    s << subDotEntry("id", _localId);

    std::ostringstream times;
    times << "(in:" << _inTime << ", out:" << _outTime << ")";
    s << subDotEntry("times", times.str());

    os << "[" << std::endl;
    os << dotEntry("type", "Edge") << ", " << std::endl;
    os << dotEntry("label", s.str()) << std::endl;
    os << "]" << std::endl;
    return os;
}
}
}
}
