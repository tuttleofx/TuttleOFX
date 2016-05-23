#include "ProcessVertexAtTime.hpp"
#include "GraphExporter.hpp"

#include <tuttle/host/ImageEffectNode.hpp>
#include <tuttle/host/InputBufferWrapper.hpp>

#include <boost/format.hpp>

namespace tuttle
{
namespace host
{
namespace graph
{

ProcessVertexAtTime::ProcessVertexAtTime()
{
}

ProcessVertexAtTime::ProcessVertexAtTime(const ProcessVertex& v, const OfxTime t)
    : IVertex(v)
    , _clipName(v._name)
    , _data(v._data, t)
{
    this->_name += "_at_" + boost::lexical_cast<std::string>(t);
}

ProcessVertexAtTime::ProcessVertexAtTime(const ProcessVertexAtTime& other)
    : IVertex(other)
    , _clipName(other._clipName)
    , _data(other._data)
{
}

ProcessVertexAtTime::~ProcessVertexAtTime()
{
}

std::ostream& ProcessVertexAtTime::exportDotDebug(std::ostream& os) const
{
    std::ostringstream s;
    s << subDotEntry("label", getName());
    if(!isFake())
    {
        /// @todo remove this. Temporary solution
        switch(getProcessNode().getNodeType())
        {
            case INode::eNodeTypeImageEffect:
            {
                const ImageEffectNode* ieNode = dynamic_cast<const ImageEffectNode*>(&getProcessNode());
                s << subDotEntry("bitdepth", ieNode->getOutputClip().getBitDepthString());
                s << subDotEntry("component", ieNode->getOutputClip().getComponentsString());
                {
                    double startFrame, endFrame;
                    ieNode->getOutputClip().getFrameRange(startFrame, endFrame);
                    s << subDotEntry("startFrame", startFrame);
                    s << subDotEntry("endFrame", endFrame);
                }
                s << subDotEntry("fps", ieNode->getOutputClip().getFrameRate());
                s << subDotEntry("output RoD", ieNode->getOutputClip().fetchRegionOfDefinition(_data._time));
                s << subDotEntry("par", ieNode->getOutputClip().getPixelAspectRatio());
                break;
            }
            default:
                break;
        }
    }
    s << subDotEntry(
        "timeDomain",
        (boost::format("[%1%:%2%]") % _data._nodeData->_timeDomain.min % _data._nodeData->_timeDomain.max).str());

    s << subDotEntry("localMemory", _data._localInfos._memory);
    s << subDotEntry("globalMemory", _data._globalInfos._memory);
    s << subDotEntry("time", _data._time);

    os << "[" << std::endl;
    os << dotEntry("type", "Node") << ", " << std::endl;
    os << dotEntry("label", s.str()) << ", " << std::endl;
    os << "]" << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const ProcessVertexAtTime& v)
{
    // Lexical_cast is used, here, to prevent an error ("error C2593: 'operator <<' is ambiguous") with ostream operator and
    // OFXTime in msvc10-express (at least).
    os << boost::lexical_cast<std::string>(v.getKey());
    return os;
}
}
}
}
