#include "ProcessVertex.hpp"
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

ProcessVertex::ProcessVertex()
    : IVertex("Undefined")
    , _data(NULL)
{
}

ProcessVertex::ProcessVertex(const ProcessVertexData& defaultVertexData, const std::string& name)
    : IVertex(name)
    , _data(defaultVertexData)
{
}

ProcessVertex::ProcessVertex(const ProcessVertex& v)
    : IVertex(v)
    , _data(v._data)
{
}

std::ostream& ProcessVertex::exportDotDebug(std::ostream& os) const
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
                if(!ieNode)
                {
                    subDotEntry("ImageEffectNode", "Bad pointer");
                    break;
                }
                s << subDotEntry("bitdepth", ieNode->getOutputClip().getBitDepthString());
                s << subDotEntry("component", ieNode->getOutputClip().getComponentsString());
                {
                    double startFrame, endFrame;
                    ieNode->getOutputClip().getFrameRange(startFrame, endFrame);
                    s << subDotEntry("startFrame", startFrame);
                    s << subDotEntry("endFrame", endFrame);
                }
                s << subDotEntry("fps", ieNode->getOutputClip().getFrameRate());
                break;
            }
            default:
                break;
        }
    }
    s << subDotEntry("timeDomain", (boost::format("[%1%:%2%]") % _data._timeDomain.min % _data._timeDomain.max).str());
    s << subDotEntry("allTimes", _data._times.size());
    std::ostringstream times;
    std::copy(_data._times.begin(), _data._times.end(), std::ostream_iterator<OfxTime>(times, ","));
    s << subDotEntry("times", times.str());

    os << "[" << std::endl;
    os << dotEntry("type", "Node") << ", " << std::endl;
    os << dotEntry("label", s.str()) << ", " << std::endl;
    os << "]" << std::endl;

    return os;
}

std::ostream& operator<<(std::ostream& os, const ProcessVertex& v)
{
    return operator<<(os, static_cast<const IVertex&>(v));
}
}
}
}
