#include "Node.hpp"
#include "Graph.hpp"

#include "Core.hpp"
#include "ImageEffectNode.hpp"

namespace tuttle
{
namespace host
{

INode* createNode(const std::string& pluginName)
{
    ofx::imageEffect::OfxhImageEffectPlugin* plug = core().getImageEffectPluginById(pluginName);

    if(!plug)
    {
        BOOST_THROW_EXCEPTION(exception::Logic() << exception::user("Plugin not found.")
                                                 //<< exception::dev( core().getImageEffectPluginCache() )
                                                 << exception::pluginIdentifier(pluginName));
    }

    plug->loadAndDescribeActions();

    ofx::imageEffect::OfxhImageEffectNode* plugInst = NULL;
    if(plug->supportsContext(kOfxImageEffectContextReader))
    {
        plugInst = plug->createInstance(kOfxImageEffectContextReader);
    }
    else if(plug->supportsContext(kOfxImageEffectContextWriter))
    {
        plugInst = plug->createInstance(kOfxImageEffectContextWriter);
    }
    else if(plug->supportsContext(kOfxImageEffectContextGeneral))
    {
        plugInst = plug->createInstance(kOfxImageEffectContextGeneral);
    }
    else if(plug->supportsContext(kOfxImageEffectContextGenerator))
    {
        plugInst = plug->createInstance(kOfxImageEffectContextGenerator);
    }
    else if(plug->supportsContext(kOfxImageEffectContextFilter))
    {
        plugInst = plug->createInstance(kOfxImageEffectContextFilter);
    }
    else
    {
        BOOST_THROW_EXCEPTION(exception::Logic()
                              << exception::user("Plugin contexts not supported by the host. (" + pluginName + ")"));
    }

    if(!plugInst)
    {
        BOOST_THROW_EXCEPTION(exception::Logic() << exception::user("Plugin not found. plugInst (" + pluginName + ")"));
    }
    ImageEffectNode* node = dynamic_cast<ImageEffectNode*>(plugInst);
    if(!node)
    {
        BOOST_THROW_EXCEPTION(exception::Logic() << exception::user("Plugin not found (" + pluginName + ")."));
    }
    return node;
}

bool compute(const std::vector<NodeInit>& nodes, const ComputeOptions& options)
{
    const_cast<ComputeOptions&>(options).setReturnBuffers(false);

    memory::MemoryCache emptyMemoryCache;
    return compute(emptyMemoryCache, nodes, options);
}

bool compute(memory::IMemoryCache& memoryCache, const std::vector<NodeInit>& nodes, const ComputeOptions& options)
{
    return compute(memoryCache, nodes, options, core().getMemoryCache());
}

bool compute(memory::IMemoryCache& memoryCache, const std::vector<NodeInit>& nodes, const ComputeOptions& options,
             memory::IMemoryCache& internMemoryCache)
{
    Graph g;
    g.addConnectedNodes(nodes);
    return g.compute(memoryCache, NodeListArg(), options, internMemoryCache);
}

NodeInit::NodeInit(const std::string& pluginName)
{
    setNode(*createNode(pluginName));
}

NodeInit::NodeInit(INode& node)
{
    setNode(node);
}

NodeInit& NodeInit::setParam(const std::string& paramName, ...)
{
    va_list ap;
    va_start(ap, paramName);

    _node->getParam(paramName).setV(ap, ofx::attribute::eChangeUserEdited);

    va_end(ap);
    return *this;
}

NodeInit& NodeInit::setParamExp(const std::string& paramName, const std::string& paramExpValue)
{
    _node->getParam(paramName).setValueFromExpression(paramExpValue, ofx::attribute::eChangeUserEdited);
    return *this;
}

void NodeInit::setBeforeRenderCallback(Callback* cb)
{
    _node->setBeforeRenderCallback(cb);
}
}
}
