#include "UEdge.hpp"

namespace tuttle
{
namespace host
{
namespace graph
{

UEdge::UEdge(const UVertex::Key& out, const UVertex::Key& in, const std::string& inAttrName)
    : IEdge(out, in, inAttrName)
{
}

std::ostream& operator<<(std::ostream& os, const UEdge& v)
{
    return operator<<(os, static_cast<const IEdge&>(v));
}
}
}
}
