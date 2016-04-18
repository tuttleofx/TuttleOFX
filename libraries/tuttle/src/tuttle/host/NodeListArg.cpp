#include "NodeListArg.hpp"
#include "INode.hpp"

#include <boost/foreach.hpp>

namespace tuttle
{
namespace host
{

NodeListArg::NodeListArg(const std::list<INode*>& nodes)
{
    BOOST_FOREACH(INode* n, nodes)
    {
        _nodes.push_back(n->getName());
    }
}

NodeListArg::NodeListArg(const INode& node)
{
    _nodes.push_back(node.getName());
}
}
}
