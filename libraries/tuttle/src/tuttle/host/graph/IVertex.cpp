#include "IVertex.hpp"
#include "GraphExporter.hpp"

#include <tuttle/host/ImageEffectNode.hpp>

namespace tuttle
{
namespace host
{
namespace graph
{

int IVertex::_count = 0;

IVertex::IVertex(const std::string& name)
    : _name(name)
    , _processNode(NULL)
    , _fake(true)
    , _used(true)
    , _id(_count++)
{
}

IVertex::IVertex(const std::string& name, INode& processNode)
    : _name(name)
    , _processNode(&processNode)
    , _fake(false)
    , _used(true)
    , _id(_count++)
{
}

IVertex::IVertex(const IVertex& v)
{
    IVertex::operator=(v);
    _id = _count++;
}

IVertex::~IVertex()
{
}

std::ostream& IVertex::exportDotDebug(std::ostream& os) const
{
    std::ostringstream s;
    s << subDotEntry("label", getName());
    if(!isFake())
    {
        /// @todo remove this. Temporary solution
        s << subDotEntry("bitdepth",
                         static_cast<const ImageEffectNode&>(getProcessNode()).getOutputClip().getBitDepthString());
    }

    os << "[" << std::endl;
    os << dotEntry("type", "Node") << ", " << std::endl;
    os << dotEntry("label", s.str()) << ", " << std::endl;
    os << "]" << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const IVertex& v)
{
    os << v.getName();
    return os;
}
}
}
}
