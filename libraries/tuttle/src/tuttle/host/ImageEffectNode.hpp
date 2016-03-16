#ifndef _TUTTLE_HOST_IMAGEEFFECTNODE_HPP_
#define _TUTTLE_HOST_IMAGEEFFECTNODE_HPP_

#include "INode.hpp"

#include <tuttle/host/attribute/Param.hpp>
#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/graph/ProcessVertexData.hpp>
#include <tuttle/host/graph/ProcessVertexAtTimeData.hpp>

#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>

#include <boost/numeric/conversion/cast.hpp>

namespace tuttle
{
namespace host
{

class ImageEffectNode : public INode, public ofx::imageEffect::OfxhImageEffectNode
{
public:
    typedef ImageEffectNode This;

public:
    ImageEffectNode(ofx::imageEffect::OfxhImageEffectPlugin& plugin, ofx::imageEffect::OfxhImageEffectNodeDescriptor& desc,
                    const std::string& context);

    ImageEffectNode(const ImageEffectNode& other);

    ~ImageEffectNode();

    std::string getLabel() const { return ofx::imageEffect::OfxhImageEffectNodeBase::getLabel(); }
    const std::string& getName() const { return ofx::imageEffect::OfxhImageEffectNodeBase::getName(); }
    void setName(const std::string& name) { return ofx::imageEffect::OfxhImageEffectNodeBase::setName(name); }
    std::size_t getNbParams() const { return ofx::attribute::OfxhParamSet::getNbParams(); }
    const ofx::attribute::OfxhParam& getParam(const std::string& name) const
    {
        return ofx::attribute::OfxhParamSet::getParam(name);
    }
    ofx::attribute::OfxhParam& getParam(const std::string& name) { return ofx::attribute::OfxhParamSet::getParam(name); }
    const ofx::attribute::OfxhParam& getParamByScriptName(const std::string& name,
                                                          const bool acceptPartialName = false) const
    {
        return ofx::attribute::OfxhParamSet::getParamByScriptName(name, acceptPartialName);
    }
    ofx::attribute::OfxhParam& getParamByScriptName(const std::string& name, const bool acceptPartialName = false)
    {
        return ofx::attribute::OfxhParamSet::getParamByScriptName(name, acceptPartialName);
    }
    const ofx::attribute::OfxhParam& getParam(const std::size_t index) const
    {
        return ofx::attribute::OfxhParamSet::getParam(index);
    }
    ofx::attribute::OfxhParam& getParam(const std::size_t index) { return ofx::attribute::OfxhParamSet::getParam(index); }

    const ofx::property::OfxhSet& getProperties() const
    {
        return ofx::imageEffect::OfxhImageEffectNodeBase::getProperties();
    }
    ofx::property::OfxhSet& getEditableProperties()
    {
        return ofx::imageEffect::OfxhImageEffectNodeBase::getEditableProperties();
    }

    std::vector<int> getVersion() const
    {
        // don't works on many plugins
        // return getProperties().fetchIntProperty( kOfxPropVersion ).getValues();
        // so get values from plugin
        std::vector<int> v;
        v.push_back(getDescriptor().getPlugin().getVersionMajor());
        v.push_back(getDescriptor().getPlugin().getVersionMinor());
        return v;
    }

    ImageEffectNode* clone() const { return new ImageEffectNode(*this); }

    bool operator==(const INode& other) const;
    bool operator==(const ImageEffectNode& other) const;

    const ENodeType getNodeType() const { return eNodeTypeImageEffect; }

    void connect(const INode& sourceEffect, attribute::Attribute& attr);

    attribute::ClipImage& getClip(const std::string& name, const bool acceptPartialName = false)
    {
        return dynamic_cast<attribute::ClipImage&>(ofx::attribute::OfxhClipImageSet::getClip(name, acceptPartialName));
    }
    const attribute::ClipImage& getClip(const std::string& name, const bool acceptPartialName = false) const
    {
        return dynamic_cast<const attribute::ClipImage&>(ofx::attribute::OfxhClipImageSet::getClip(name, acceptPartialName));
    }

    attribute::ClipImage& getOutputClip() { return getClip(kOfxImageEffectOutputClipName); }
    const attribute::ClipImage& getOutputClip() const { return getClip(kOfxImageEffectOutputClipName); }

    attribute::Attribute& getAttribute(const std::string& name) { return getClip(name); }
    attribute::Attribute& getSingleInputAttribute();

    const attribute::Attribute& getSingleInputAttribute() const
    {
        return const_cast<ImageEffectNode*>(this)->getSingleInputAttribute();
    }

    ofx::attribute::OfxhParamSet& getParamSet() { return *this; }
    const ofx::attribute::OfxhParamSet& getParamSet() const { return *this; }

    ofx::attribute::OfxhClipImageSet& getClipImageSet() { return *this; }
    const ofx::attribute::OfxhClipImageSet& getClipImageSet() const { return *this; }

    std::size_t getLocalHashAtTime(const OfxTime time) const;

    OfxRectD getRegionOfDefinition(const OfxTime time) const { return getData(time)._apiImageEffect._renderRoD; }

    OfxRangeD getTimeDomain() const { return getData()._timeDomain; }

    void debugOutputImage(const OfxTime time) const;

    OfxRangeD getDefaultTimeDomain() const;

    /// @group Implementation of INode virtual functions
    /// @{
    OfxRangeD computeTimeDomain();

    void setup1();
    void setup2_reverse();
    void setup3();

    void beginSequence(graph::ProcessVertexData& vData);

    INode::ClipTimesSetMap getTimesNeeded(const OfxTime time) const { return OfxhImageEffectNode::getFramesNeeded(time); }

    void preProcess1(graph::ProcessVertexAtTimeData& vData);
    void preProcess2_reverse(graph::ProcessVertexAtTimeData& vData);

    bool isIdentity(const graph::ProcessVertexAtTimeData& vData, std::string& clip, OfxTime& time) const;
    void preProcess_infos(const graph::ProcessVertexAtTimeData& vData, const OfxTime time,
                          graph::ProcessVertexAtTimeInfo& nodeInfos) const;
    void process(graph::ProcessVertexAtTimeData& vData);
    void postProcess(graph::ProcessVertexAtTimeData& vData);

    void endSequence(graph::ProcessVertexData& vData);
    /// @}

    std::ostream& print(std::ostream& os) const;

    friend std::ostream& operator<<(std::ostream& os, const This& v);

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // overridden for imageEffect::OfxhImageEffectNode

    /// get default output fielding. This is passed into the clip prefs action
    /// and  might be mapped (if the host allows such a thing)
    inline const std::string& getDefaultOutputFielding() const { return _defaultOutputFielding; }

    /**
     * @return 1 to abort processing
     */
    int abort();

    /**
     * Allocating memory using the memoryPool
     */
    ofx::OfxhMemory* newMemoryInstance(size_t nBytes);

    /// make a clip
    ofx::attribute::OfxhClipImage* newClipImage(const ofx::attribute::OfxhClipImageDescriptor& descriptor);

#ifndef SWIG
    /// vmessage
    void vmessage(const char* type, const char* id, const char* format, va_list args) const OFX_EXCEPTION_SPEC;
#endif

    // The size of the current project in canonical coordinates.
    // The size of a project is a sub set of the kOfxImageEffectPropProjectExtent. For example a
    // project may be a PAL SD project, but only be a letter-box within that. The project size is
    // the size of this sub window.
    void getProjectSize(double& xSize, double& ySize) const;

    // The offset of the current project in canonical coordinates.
    // The offset is related to the kOfxImageEffectPropProjectSize and is the offset from the origin
    // of the project 'subwindow'. For example for a PAL SD project that is in letterbox form, the
    // project offset is the offset to the bottom left hand corner of the letter box. The project
    // offset is in canonical coordinates.
    void getProjectOffset(double& xOffset, double& yOffset) const;

    // The extent of the current project in canonical coordinates.
    // The extent is the size of the 'output' for the current project. See ProjectCoordinateSystems
    // for more infomation on the project extent. The extent is in canonical coordinates and only
    // returns the top right position, as the extent is always rooted at 0,0. For example a PAL SD
    // project would have an extent of 768, 576.
    void getProjectExtent(double& xSize, double& ySize) const;

    // The pixel aspect ratio of the current project
    double getProjectPixelAspectRatio() const;

    // The pixel components type of the current project
    const std::string getProjectPixelComponentsType() const;

    // The pixel bit depth of the current project
    const std::string getProjectBitDepth() const;

    // The duration of the effect
    // This contains the duration of the plug-in effect, in frames.
    double getEffectDuration() const;

    /// This is called whenever a param is changed by the plugin so that
    /// the recursive instanceChangedAction will be fed the correct frame
    double getFrameRecursive() const;

    /// This is called whenever a param is changed by the plugin so that
    /// the recursive instanceChangedAction will be fed the correct
    /// renderScale
    void getRenderScaleRecursive(double& x, double& y) const;

    /// Create a parameter instance
    ofx::attribute::OfxhParam* newParam(const ofx::attribute::OfxhParamDescriptor& Descriptor) OFX_EXCEPTION_SPEC;

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // overridden for Param::SetInstance

    /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
    ///
    /// Client host code needs to implement this
    void editBegin(const std::string& name) OFX_EXCEPTION_SPEC;

    /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
    ///
    /// Client host code needs to implement this
    void editEnd() OFX_EXCEPTION_SPEC;

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // overridden for OfxhIProgress

    /// Start doing progress.
    void progressStart(const std::string& message);

    /// finish yer progress
    void progressEnd();

    /// set the progress to some level of completion,
    /// returns true if you should abandon processing, false to continue
    bool progressUpdate(const double t);

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // overridden for OfxhITimeLine

    /// get the current time on the timeline. This is not necessarily the same
    /// time as being passed to an action (eg render)
    double timelineGetTime();

    /// set the timeline to a specific time
    void timelineGotoTime(double t);

    /// get the first and last times available on the effect's timeline
    void timelineGetBounds(double& t1, double& t2);

    const OfxRangeD& getEffectFrameRange() const { return getData()._renderTimeRange; }

    void beginSequenceRenderAction(OfxTime startFrame, OfxTime endFrame, double step, bool interactive,
                                   OfxPointD renderScale) OFX_EXCEPTION_SPEC;

private:
    void checkClipsConnected() const;

    void initComponents();
    void initInputClipsPixelAspectRatio();
    void initPixelAspectRatio();
    void initInputClipsFps();
    void initFps();

    void maximizeBitDepthFromReadsToWrites();
    void maximizeBitDepthFromWritesToReads();
    void coutBitDepthConnections() const;
    void validInputClipsConnections() const;

    /// our clip is pretending to be progressive PAL SD, so return kOfxImageFieldNone
    std::string _defaultOutputFielding;
};
}
}

#endif
