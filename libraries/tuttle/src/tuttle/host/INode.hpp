#ifndef _TUTTLE_HOST_INODE_HPP_
#define _TUTTLE_HOST_INODE_HPP_

#include <tuttle/host/exceptions.hpp>

#include <ofxCore.h>
#include <ofxAttribute.h>
#include <tuttle/host/Callback.hpp>

#include <boost/noncopyable.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>

namespace tuttle
{
namespace host
{

class Callback;

namespace ofx
{
namespace attribute
{
class OfxhParam;
class OfxhParamSet;
class OfxhClipImageSet;
}
namespace property
{
class OfxhSet;
}
}
namespace attribute
{
class Attribute;
class Clip;
class ClipImage;
}
namespace graph
{
class ProcessVertexAtTimeData;
class ProcessVertexData;
class ProcessVertexAtTimeInfo;
}

class ImageEffectNode;

class INode : private boost::noncopyable
{
public:
    typedef INode This;

    typedef std::set<OfxTime> TimesSet;
    typedef std::map<std::string, TimesSet> ClipTimesSetMap;

    enum ENodeType
    {
        eNodeTypeUnknown,
        eNodeTypeImageEffect,
        eNodeTypeParam,
        eNodeTypeGraph,
        eNodeTypeBuffer,
    };

public:
    INode()
        : _data(NULL)
        , _beforeRenderCallback(0)
    {
    }
    INode(const INode& e)
        : _data(NULL)
        , _beforeRenderCallback(0)
    {
    }

    virtual ~INode();
    virtual INode* clone() const = 0;
    virtual bool operator==(const INode&) const = 0;

    virtual const std::string& getName() const = 0;
    virtual void setName(const std::string& name) = 0;
    virtual const ENodeType getNodeType() const = 0;

    ImageEffectNode& asImageEffectNode();
    const ImageEffectNode& asImageEffectNode() const;

    virtual std::vector<int> getVersion() const = 0;
    std::string getVersionStr() const;

    virtual std::string getLabel() const = 0;

    virtual const ofx::property::OfxhSet& getProperties() const = 0;
    virtual ofx::property::OfxhSet& getEditableProperties() = 0;

    virtual attribute::Attribute& getAttribute(const std::string& name) = 0;
    const attribute::Attribute& getAttribute(const std::string& name) const
    {
        return const_cast<INode*>(this)->getAttribute(name);
    }
    virtual attribute::Attribute& getSingleInputAttribute() = 0;
    virtual const attribute::Attribute& getSingleInputAttribute() const = 0;

    virtual std::size_t getNbParams() const = 0;
    virtual const ofx::attribute::OfxhParam& getParam(const std::string& name) const = 0;
    virtual ofx::attribute::OfxhParam& getParam(const std::string& name) = 0;
    virtual const ofx::attribute::OfxhParam& getParamByScriptName(const std::string& name,
                                                                  const bool acceptPartialName = false) const = 0;
    virtual ofx::attribute::OfxhParam& getParamByScriptName(const std::string& name,
                                                            const bool acceptPartialName = false) = 0;
    virtual const ofx::attribute::OfxhParam& getParam(const std::size_t index) const = 0;
    virtual ofx::attribute::OfxhParam& getParam(const std::size_t index) = 0;

    virtual attribute::ClipImage& getClip(const std::string& name, const bool acceptPartialName = false) = 0;
    virtual const attribute::ClipImage& getClip(const std::string& name, const bool acceptPartialName = false) const = 0;

    attribute::ClipImage& getOutputClip() { return getClip(kOfxImageEffectOutputClipName); }
    const attribute::ClipImage& getOutputClip() const { return getClip(kOfxImageEffectOutputClipName); }

    virtual ofx::attribute::OfxhParamSet& getParamSet() = 0;
    virtual const ofx::attribute::OfxhParamSet& getParamSet() const = 0;

    virtual ofx::attribute::OfxhClipImageSet& getClipImageSet() = 0;
    virtual const ofx::attribute::OfxhClipImageSet& getClipImageSet() const = 0;

    virtual std::size_t getLocalHashAtTime(const OfxTime time) const = 0;

#ifndef SWIG
    virtual void connect(const INode&, attribute::Attribute&) = 0;

    virtual void setup1() = 0;
    virtual void setup2_reverse() = 0;
    virtual void setup3() = 0;

    virtual OfxRangeD computeTimeDomain() = 0;

    //	virtual OfxTime mapInputTime( const OfxTime time ) const = 0;

    virtual OfxRangeD getTimeDomain() const = 0;

    /**
     * @brief Begin of the a new frame range to process. Initilize this node.
     * @param[in] processData
     * @remark called on each node without predefined order.
     */
    virtual void beginSequence(graph::ProcessVertexData& processData) = 0;

    /**
     * @brief Asks the plugin all times it needs for each of it's input clips.
     * @param[in] time
     */
    virtual ClipTimesSetMap getTimesNeeded(const OfxTime time) const = 0;

    /**
     * @brief Initialization pass to propagate informations from inputs to outputs.
     * @param[in] processData
     * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each
     * input nodes before.
     */
    virtual void preProcess1(graph::ProcessVertexAtTimeData& processData) {}

    /**
     * @brief Initialization pass to propagate informations from outputs to inputs.
     * @param[in] processData
     * @remark Called on each node in a REVERSE depth first search order. So you have the guarantee that it has been called
     * on each output nodes before. Output nodes are those who used the result of the current node.
     */
    virtual void preProcess2_reverse(graph::ProcessVertexAtTimeData& processData) {}

    /**
     * @brief The node can declare to be an identity operation.
     * In this case, the node is not processed and the rendering engine direcly use the indicated input clip at a particular
     * time.
     *
     * @param[in] processData
     * @param[out] clip the input clip to use as identity
     * @param[out] time the time to use as identity
     * @return if the node is an identity operation
     */
    virtual bool isIdentity(const graph::ProcessVertexAtTimeData& processData, std::string& clip, OfxTime& time) const = 0;

    /**
     * @brief Fill ProcessInfo to compute statistics for the current process,
     *        like memory used by this node, by all input nodes, etc.
     * @param[in] processData
     * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each
     * input nodes before.
     */
    virtual void preProcess_infos(const graph::ProcessVertexAtTimeData& processData, const OfxTime time,
                                  graph::ProcessVertexAtTimeInfo& nodeInfos) const = 0;

    /**
     * @brief Process this node. All inputs are compute.
     * @param[in] processData
     * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each
     * input nodes before.
     */
    virtual void process(graph::ProcessVertexAtTimeData& processData) = 0;

    /**
     * @brief The process of all nodes is done for one frame, now finalize this node.
     * @param[in] processData
     * @remark Called on each node in a depth first search order. So you have the guarantee that it has been called on each
     * input nodes before.
     */
    virtual void postProcess(graph::ProcessVertexAtTimeData& processData) = 0;

    /**
     * @brief End of the whole frame range process, now finalize this node.
     * @param[in] processData
     * @remark called on each node without predefined order.
     */
    virtual void endSequence(graph::ProcessVertexData& processData) = 0;

#endif

    /**
     * Callback triggered before the render. The Node does not own the callback
     * so the client has to set the callback to null and destroy it.
     */
    void setBeforeRenderCallback(Callback* cb);

    virtual std::ostream& print(std::ostream& os) const = 0;

    friend std::ostream& operator<<(std::ostream& os, const This& v);

#ifndef SWIG
    typedef graph::ProcessVertexData Data;
    typedef graph::ProcessVertexAtTimeData DataAtTime;
    typedef std::map<OfxTime, DataAtTime*> DataAtTimeMap;

    // Callbacks hidden from Swig and with the DataAtTime definition available
    void beforeRenderCallback(INode&, DataAtTime&);
    Callback* _beforeRenderCallback;

protected:
    Data* _data;               ///< link to external datas
    DataAtTimeMap _dataAtTime; ///< link to external datas at each time

public:
    void setProcessData(Data* data);
    void setProcessDataAtTime(DataAtTime* dataAtTime);
    void clearProcessDataAtTime();

    Data& getData();
    const Data& getData() const;

    bool hasData(const OfxTime time) const;
    const DataAtTime& getData(const OfxTime time) const;
    const DataAtTime& getFirstData() const;
    const DataAtTime& getLastData() const;
    DataAtTime& getData(const OfxTime time);
    DataAtTime& getFirstData();
    DataAtTime& getLastData();
#endif
};

#ifndef SWIG
/**
 * @brief to make clonable for use in boost::ptr_container.
 */
inline INode* new_clone(const INode& a)
{
    return a.clone();
}

#endif

inline std::string mapNodeTypeEnumToString(const INode::ENodeType e)
{
    switch(e)
    {
        case INode::eNodeTypeUnknown:
            return "Unknown";
        case INode::eNodeTypeImageEffect:
            return "ImageEffect";
        case INode::eNodeTypeParam:
            return "Param";
        case INode::eNodeTypeGraph:
            return "Graph";
        case INode::eNodeTypeBuffer:
            return "Buffer";
    }
    BOOST_THROW_EXCEPTION(exception::Bug() << exception::dev() + "Unrecognized ENodeType enum. (" + e + ").");
}
}
}

#endif
