#include "ProcessEdge.hpp"
#include "GraphExporter.hpp"

namespace tuttle
{
namespace host
{
namespace graph
{

ProcessEdge::ProcessEdge()
{
}

ProcessEdge::ProcessEdge(const ProcessVertex::Key& out, const ProcessVertex::Key& in, const std::string& inAttrName)
    : IEdge(out, in, inAttrName)
{
}

ProcessEdge::ProcessEdge(const ProcessEdge& e)
    : IEdge(e)
{
}

ProcessEdge::~ProcessEdge()
{
}

std::ostream& ProcessEdge::exportDotDebug(std::ostream& os) const
{
    std::ostringstream s;
    s << subDotEntry("label", getName());
    s << subDotEntry("id", _localId);
    std::ostringstream timesNeeded;
    timesNeeded << "{";
    BOOST_FOREACH(const TimeMap::value_type& m, _timesNeeded)
    {
        timesNeeded << m.first << ": (";
        std::copy(m.second.begin(), m.second.end(), std::ostream_iterator<OfxTime>(timesNeeded, ","));
        timesNeeded << "), ";
    }
    timesNeeded << "}";
    s << subDotEntry("timesNeeded", timesNeeded.str());

    os << "[" << std::endl;
    os << dotEntry("type", "Edge") << ", " << std::endl;
    os << dotEntry("label", s.str()) << std::endl;
    os << "]" << std::endl;
    return os;
}
}
}
}
