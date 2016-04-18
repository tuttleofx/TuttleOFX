#ifndef _TUTTLE_HOST_PROCESSVERTEXATTIMEDATA_HPP_
#define _TUTTLE_HOST_PROCESSVERTEXATTIMEDATA_HPP_

#include "ProcessVertexData.hpp"

#include <tuttle/host/ofx/attribute/OfxhClipImage.hpp>
#include <tuttle/host/ofx/OfxhCore.hpp>

#include <string>

namespace tuttle
{
namespace host
{
namespace graph
{

class ProcessEdgeAtTime;

class ProcessVertexAtTimeInfo
{
    typedef ProcessVertexAtTimeInfo This;

public:
    ProcessVertexAtTimeInfo()
        : _nodes(0)
        , _memory(0)
    {
    }

    std::size_t _nodes;
    std::size_t _memory;

    ProcessVertexAtTimeInfo& operator+=(const ProcessVertexAtTimeInfo& p)
    {
        _nodes += p._nodes;
        _memory += p._memory;
        return *this;
    }

public:
    friend std::ostream& operator<<(std::ostream& os, const This& g);
};

class ProcessVertexAtTimeData
{
    typedef ProcessVertexAtTimeData This;

public:
    ProcessVertexAtTimeData()
        : _nodeData(NULL)
        , _time(0)
        , _isFinalNode(false)
        , _outDegree(0)
        , _inDegree(0)
    {
        _localInfos._nodes = 1; // local infos can contain only 1 node by definition...
    }

    ProcessVertexAtTimeData(const ProcessVertexData& nodeData, const OfxTime time)
        : _nodeData(&nodeData)
        , _time(time)
        , _isFinalNode(false)
        , _outDegree(0)
        , _inDegree(0)
    {
        _localInfos._nodes = 1; // local infos can contain only 1 node by definition...
    }

    ProcessVertexAtTimeData(const This& other)
        : _nodeData(other._nodeData)
    {
        operator=(other);
    }
    ~ProcessVertexAtTimeData() {}

    This& operator=(const This& v)
    {
        _nodeData = v._nodeData;

        _time = v._time;
        _isFinalNode = v._isFinalNode;
        _outDegree = v._outDegree;
        _inDegree = v._inDegree;
        _localInfos = v._localInfos;
        _inputsInfos = v._inputsInfos;
        _globalInfos = v._globalInfos;

        _apiImageEffect = v._apiImageEffect;

        return *this;
    }

    inline const ProcessEdgeAtTime& getInputEdgeByClipName(const std::string& inputClipName, const OfxTime time) const
    {
        Key k(inputClipName, time);
        ProcessEdgeAtTimeByClipName::const_iterator it = _inEdges.find(k);
        if(it == _inEdges.end())
            BOOST_THROW_EXCEPTION(exception::Bug() << exception::dev() + "No input clip " + quotes(inputClipName) +
                                                          " at time " + time + " inside input edges map.");
        return *(it->second);
    }

public:
    friend std::ostream& operator<<(std::ostream& os, const This& vData);

public:
    const ProcessVertexData* _nodeData;

    OfxTime _time;
    bool _isFinalNode;

    typedef std::pair<std::string, OfxTime> Key;
    typedef std::map<Key, const ProcessEdgeAtTime*> ProcessEdgeAtTimeByClipName;
    ProcessEdgeAtTimeByClipName _inEdges;
    std::vector<const ProcessEdgeAtTime*> _outEdges;

    std::size_t _outDegree; ///< number of connected input clips
    std::size_t _inDegree;  ///< number of nodes using the output of this node

    ProcessVertexAtTimeInfo _localInfos;
    ProcessVertexAtTimeInfo _inputsInfos;
    ProcessVertexAtTimeInfo _globalInfos;

    /// @group API Specific datas
    /// @{
    /**
     * @brief imageEffect specific process datas
     */
    struct ImageEffect
    {
        std::string _field;
        OfxRectD _renderRoD; // is it a good thing to store this here ?
        OfxRectD _renderRoI;

        typedef std::map<tuttle::host::ofx::attribute::OfxhClipImage*, OfxRectD> MapClipImageRod;
        MapClipImageRod _inputsRoI; ///<< in which the plugin set the RoI it needs for each input clip

    } _apiImageEffect;
    /// @}
};
}
}
}

#endif
