#include "IEdge.hpp"
#include "GraphExporter.hpp"

namespace tuttle
{
namespace host
{
namespace graph
{

IEdge::IEdge()
    : _localId(0)
{
}

IEdge::IEdge(const std::string& out, const std::string& in, const std::string& inAttrName)
    : _out(out)
    , _in(in)
    , _inAttrName(inAttrName)
    , _name(std::string(out) + "." + kOfxOutputAttributeName + "-->" + in + "." + inAttrName)
    , _localId(0)
{
}

IEdge::IEdge(const IEdge& e)
{
    this->operator=(e);
    //_id = _count++;
}

IEdge::~IEdge()
{
}

std::ostream& IEdge::exportDotDebug(std::ostream& os) const
{
    std::ostringstream s;
    s << subDotEntry("label", getName());
    s << subDotEntry("id", _localId);

    os << "[" << std::endl;
    os << dotEntry("type", "Edge") << ", " << std::endl;
    os << dotEntry("label", s.str()) << std::endl;
    os << "]" << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const IEdge& v)
{
    os << v.getName();
    return os;
}
}
}
}
