#ifndef _TUTTLE_HOST_NODE_HPP_
#define _TUTTLE_HOST_NODE_HPP_

#include "INode.hpp"
#include "ComputeOptions.hpp"
#include "memory/MemoryCache.hpp"

#include <boost/assign/list_of.hpp>

#include <memory>

namespace tuttle
{
namespace host
{

class NodeInit;

using boost::assign::list_of;

INode* createNode(const std::string& pluginName);

bool compute(const std::vector<NodeInit>& nodes, const ComputeOptions& options = ComputeOptions());

bool compute(memory::IMemoryCache& memoryCache, const std::vector<NodeInit>& nodes,
             const ComputeOptions& options = ComputeOptions());

bool compute(memory::IMemoryCache& memoryCache, const std::vector<NodeInit>& nodes, const ComputeOptions& options,
             memory::IMemoryCache& internMemoryCache);

/**
 * @brief Node initializer class.
 */
class NodeInit
{
public:
    NodeInit() {}
    NodeInit(const std::string& pluginName);
    NodeInit(INode& node);
    /**
     * @brief Non-standard copy contructor that steals the data.
     */
    NodeInit(const NodeInit& other) { setNode(other.release()); }

    NodeInit& operator=(const NodeInit& other)
    {
        setNode(other.release());
        return *this;
    }

    INode& operator->() { return *_node.get(); }
    const INode& operator->() const { return *_node.get(); }

    /**
     * @brief Set parameter values. If it's a multi-dimensional parameter,
     * you should put all dimensions values.
     * @exemple setParam("redColor", 1.0, 0.0, 0.0, 1.0)
     */
    NodeInit& setParam(const char* paramName, ...);

    /**
     * @brief Set parameter value from a string expression.
     */
    NodeInit& setParamExp(const std::string& paramName, const std::string& paramValue);

    const INode& get() const { return *_node; }
    INode& get() { return *_node; }

    void setNode(INode& node) { _node.reset(&node); }
    INode& release() const { return *_node.release(); }

    void setBeforeRenderCallback(Callback* cb);

private:
    mutable std::auto_ptr<INode> _node;
};
}
}

#endif
