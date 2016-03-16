#include "ImageEffectNode.hpp"
#include "HostDescriptor.hpp"

// ofx host
#include <tuttle/host/Core.hpp> // for core().getMemoryCache()
#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/attribute/allParams.hpp>
#include <tuttle/host/graph/ProcessEdgeAtTime.hpp>
#include <tuttle/host/graph/ProcessVertexData.hpp>
#include <tuttle/host/graph/ProcessVertexAtTimeData.hpp>

#include <tuttle/host/ofx/OfxhUtilities.hpp>
#include <tuttle/host/ofx/OfxhBinary.hpp>
#include <tuttle/host/ofx/OfxhMemory.hpp>
#include <tuttle/host/ofx/OfxhImageEffectNode.hpp>
#include <tuttle/host/ofx/OfxhPluginAPICache.hpp>
#include <tuttle/host/ofx/OfxhPluginCache.hpp>
#include <tuttle/host/ofx/OfxhHost.hpp>
#include <tuttle/host/ofx/OfxhImageEffectPlugin.hpp>
#include <tuttle/host/ofx/property/OfxhSet.hpp>
#include <tuttle/host/ofx/attribute/OfxhClip.hpp>
#include <tuttle/host/ofx/attribute/OfxhParam.hpp>

// ofx
#include <ofxCore.h>
#include <ofxImageEffect.h>

#include <boost/functional/hash.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <list>

namespace tuttle
{
namespace host
{

ImageEffectNode::ImageEffectNode(tuttle::host::ofx::imageEffect::OfxhImageEffectPlugin& plugin,
                                 tuttle::host::ofx::imageEffect::OfxhImageEffectNodeDescriptor& desc,
                                 const std::string& context)
    : tuttle::host::ofx::imageEffect::OfxhImageEffectNode(plugin, desc, context, false)
    , _defaultOutputFielding(kOfxImageFieldNone)
{
    populate();
    //	createInstanceAction();
}

ImageEffectNode::ImageEffectNode(const ImageEffectNode& other)
    : INode(other)
    , tuttle::host::ofx::imageEffect::OfxhImageEffectNode(other)
    , _defaultOutputFielding(other._defaultOutputFielding)
{
    populate();
    copyAttributesValues(other); // values need to be setted before the createInstanceAction !
    createInstanceAction();
}

ImageEffectNode::~ImageEffectNode()
{
}

bool ImageEffectNode::operator==(const INode& other) const
{
    const ImageEffectNode* other_ptr = dynamic_cast<const ImageEffectNode*>(&other);
    if(other_ptr == NULL)
        return false;
    return operator==(*other_ptr);
}

/**
 * @warning do a deep comparison
 */
bool ImageEffectNode::operator==(const ImageEffectNode& other) const
{
    return ofx::imageEffect::OfxhImageEffectNode::operator==(other);
}

void ImageEffectNode::connect(const INode& sourceEffect, attribute::Attribute& attr)
{
    const attribute::ClipImage& outputClip =
        dynamic_cast<const attribute::ClipImage&>(sourceEffect.getClip(kOfxImageEffectOutputClipName));
    attribute::ClipImage& inputClip =
        dynamic_cast<attribute::ClipImage&>(attr); // throw an exception if not a ClipImage attribute

    inputClip.setConnectedClip(outputClip);
}

attribute::Attribute& ImageEffectNode::getSingleInputAttribute()
{
    ofx::attribute::OfxhClipImageSet::ClipImageVector& clips = getClipsByOrder();
    ofx::attribute::OfxhClipImageSet::ClipImageMap& clipsMap = getClipsByName();
    ofx::attribute::OfxhAttribute* inAttr = NULL;

    if(clips.size() == 1)
    {
        inAttr = &clips[0];
    }
    else if(clips.size() > 1)
    {
        const ofx::attribute::OfxhClipImageSet::ClipImageMap::iterator it(clipsMap.find(kOfxSimpleSourceAttributeName));
        if(it != clipsMap.end())
        {
            inAttr = it->second;
        }
        else
        {
            inAttr = &clips[0];
        }
    }
    else // if( inClips.empty() )
    {
        BOOST_THROW_EXCEPTION(exception::Logic() << exception::user("No source clip."));
    }
    return dynamic_cast<attribute::ClipImage&>(*inAttr);
}

// get a new clip instance
tuttle::host::ofx::attribute::OfxhClipImage*
ImageEffectNode::newClipImage(const tuttle::host::ofx::attribute::OfxhClipImageDescriptor& descriptor)
{
    return new attribute::ClipImage(*this, descriptor);
}

std::size_t ImageEffectNode::getLocalHashAtTime(const OfxTime time) const
{
    std::size_t seed = getPlugin().getHash();

    if(isFrameVarying())
    {
        boost::hash_combine(seed, time);
    }

    boost::hash_combine(seed, getParamSet().getHashAtTime(time));

    return seed;
}

/**
 * @return 1 to abort processing
 */
int ImageEffectNode::abort()
{
    return 0;
}

ofx::OfxhMemory* ImageEffectNode::newMemoryInstance(size_t nBytes)
{
    ofx::OfxhMemory* instance = new ofx::OfxhMemory();

    instance->alloc(nBytes);
    return instance;
}

// vmessage
void ImageEffectNode::vmessage(const char* type, const char* id, const char* format, va_list args) const OFX_EXCEPTION_SPEC
{
    vprintf(format, args);
}

// get the project size in CANONICAL pixels, so PAL SD return 768, 576
void ImageEffectNode::getProjectSize(double& xSize, double& ySize) const
{
    if(_dataAtTime.size() == 0)
    {
        xSize = 720;
        ySize = 576;
    }
    else
    {
        OfxRectD rod = getLastData()._apiImageEffect._renderRoD;
        xSize = rod.x2 - rod.x1;
        ySize = rod.y2 - rod.y1;
        if(xSize < 1 || ySize < 1)
        {
            xSize = 720;
            ySize = 576;
        }
    }
}

// get the project offset in CANONICAL pixels, we are at 0,0
void ImageEffectNode::getProjectOffset(double& xOffset, double& yOffset) const
{
    xOffset = 0;
    yOffset = 0;
}

// get the project extent in CANONICAL pixels, so PAL SD return 768, 576
void ImageEffectNode::getProjectExtent(double& xSize, double& ySize) const
{
    if(_dataAtTime.size() == 0)
    {
        xSize = 720;
        ySize = 576;
    }
    else
    {
        OfxRectD rod = getLastData()._apiImageEffect._renderRoD;
        xSize = rod.x2 - rod.x1;
        ySize = rod.y2 - rod.y1;
        if(xSize < 1 || ySize < 1)
        {
            xSize = 720;
            ySize = 576;
        }
    }
}

// get the PAR, SD PAL is 768/720=1.0666
double ImageEffectNode::getProjectPixelAspectRatio() const
{
    return 1.0;
}

// we are only 25 frames
double ImageEffectNode::getEffectDuration() const
{
    return 99999.0;
}

/// This is called whenever a param is changed by the plugin so that
/// the recursive instanceChangedAction will be fed the correct frame
double ImageEffectNode::getFrameRecursive() const
{
    return 0.0;
}

/// This is called whenever a param is changed by the plugin so that
/// the recursive instanceChangedAction will be fed the correct
/// renderScale
void ImageEffectNode::getRenderScaleRecursive(double& x, double& y) const
{
    x = y = 1.0;
}

/**
 * The pixel components type of the current project
 * @todo tuttle: to remove in the future.... size, pixelType, BitDepth, etc... must be locally defined
 */
const std::string ImageEffectNode::getProjectPixelComponentsType() const
{
    return kOfxImageComponentRGBA;
}

/**
 * The pixel bit depth of the current project (host work in float)
 * @todo tuttle: to remove in the future.... size, pixelType, BitDepth, etc... must be locally defined
 */
const std::string ImageEffectNode::getProjectBitDepth() const
{
    // return kOfxBitDepthByte;
    return kOfxBitDepthFloat;
}

// make a parameter instance
ofx::attribute::OfxhParam*
ImageEffectNode::newParam(const ofx::attribute::OfxhParamDescriptor& descriptor) OFX_EXCEPTION_SPEC
{
    const std::string name = descriptor.getName();
    ofx::attribute::OfxhParam* param = NULL;

    try
    {
        if(descriptor.getParamType() == kOfxParamTypeString)
            param = new attribute::ParamString(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypeInteger)
            param = new attribute::ParamInteger(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypeDouble)
            param = new attribute::ParamDouble(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypeBoolean)
            param = new attribute::ParamBoolean(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypeChoice)
            param = new attribute::ParamChoice(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypeRGBA)
            param = new attribute::ParamRGBA(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypeRGB)
            param = new attribute::ParamRGB(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypeDouble2D)
            param = new attribute::ParamDouble2D(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypeDouble3D)
            param = new attribute::ParamDouble3D(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypeInteger2D)
            param = new attribute::ParamInteger2D(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypeInteger3D)
            param = new attribute::ParamInteger3D(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypePushButton)
            param = new attribute::ParamPushButton(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypeGroup)
            param = new attribute::ParamGroup(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypePage)
            param = new attribute::ParamPage(*this, name, descriptor);
        else if(descriptor.getParamType() == kOfxParamTypeCustom)
            param = new attribute::ParamCustom(*this, name, descriptor);
        else
        {
            BOOST_THROW_EXCEPTION(exception::Failed() << exception::user() + "Can't create param " + quotes(name) +
                                                             " instance from param descriptor, type not recognized.");
        }
        this->addParam(param);
    }
    catch(exception::Common& e)
    {
        BOOST_THROW_EXCEPTION(
            ofx::OfxhException(*boost::get_error_info<exception::ofxStatus>(e), boost::diagnostic_information(e)));
    }
    catch(...)
    {
        BOOST_THROW_EXCEPTION(ofx::OfxhException(kOfxStatErrUnknown, boost::current_exception_diagnostic_information()));
    }
    return param;
}

void ImageEffectNode::editBegin(const std::string& name) OFX_EXCEPTION_SPEC
{
    // BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrMissingHostFeature ) );
}

void ImageEffectNode::editEnd() OFX_EXCEPTION_SPEC
{
    // BOOST_THROW_EXCEPTION( ofx::OfxhException( kOfxStatErrMissingHostFeature ) );
}

/// Start doing progress.
void ImageEffectNode::progressStart(const std::string& message)
{
    // TUTTLE_LOG_TRACE( message );
    if(!(getContext() == kOfxImageEffectContextReader) && !(getContext() == kOfxImageEffectContextWriter))
        TUTTLE_LOG_INFO(std::left << "       " << common::Color::get()->_green << std::setw(TUTTLE_LOG_PLUGIN_NAME_WIDTH)
                                  << getName() << common::Color::get()->_std);
}

/// finish yer progress
void ImageEffectNode::progressEnd()
{
    // std::cout << std::endl;
}

/// set the progress to some level of completion,
/// returns true if you should abandon processing, false to continue
bool ImageEffectNode::progressUpdate(const double progress)
{
    /*
    if( ( getContext() == kOfxImageEffectContextReader ) || ( getContext() == kOfxImageEffectContextWriter ) )
            TUTTLE_LOG_INFO( "\r" << common::Color::get()->_std << "[" << std::right << std::setw(3) << int(progress * 100)
    << "%] " << " " << std::left << std::flush );
    else
            TUTTLE_LOG_INFO( "\r" << common::Color::get()->_std << "[" << std::right << std::setw(3) << int(progress * 100)
    << "%] " << std::left << common::Color::get()->_green << getName() << common::Color::get()->_std << std::flush );
            */
    return false;
}

/// get the current time on the timeline. This is not necessarily the same
/// time as being passed to an action (eg render)
double ImageEffectNode::timelineGetTime()
{
    return 0;
}

/// set the timeline to a specific time
void ImageEffectNode::timelineGotoTime(double t)
{
}

/// get the first and last times available on the effect's timeline
void ImageEffectNode::timelineGetBounds(double& t1, double& t2)
{
    t1 = 0;
    t2 = 99999;
}

/// override to get frame range of the effect
void ImageEffectNode::beginSequenceRenderAction(OfxTime startFrame, OfxTime endFrame, double step, bool interactive,
                                                OfxPointD renderScale) OFX_EXCEPTION_SPEC
{
    OfxhImageEffectNode::beginSequenceRenderAction(startFrame, endFrame, step, interactive, renderScale);
}

void ImageEffectNode::checkClipsConnected() const
{
    for(ClipImageMap::const_iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
    {
        const attribute::ClipImage& clip = dynamic_cast<const attribute::ClipImage&>(*(it->second));
        if(!clip.isOutput() && !clip.isConnected() && !clip.isOptional()) // one non optional input clip is unconnected
        {
            BOOST_THROW_EXCEPTION(exception::Logic()
                                  << exception::user("A non optional clip is unconnected ! (" + clip.getFullName() + ")"));
        }
    }
}

void ImageEffectNode::initComponents()
{
    attribute::ClipImage& outputClip = dynamic_cast<attribute::ClipImage&>(getOutputClip());
    // bool inputClipsFound                = false;
    std::string mostChromaticComponents = kOfxImageComponentNone;

    for(ClipImageMap::iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
    {
        attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(it->second));
        if(!clip.isOutput() && clip.isConnected())
        {
            // inputClipsFound = true;
            const attribute::ClipImage& linkClip = clip.getConnectedClip();
            mostChromaticComponents = findMostChromaticComponents(linkClip.getComponentsString(), mostChromaticComponents);
        }
    }
    // components
    for(ClipImageMap::iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
    {
        attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(it->second));
        if(!clip.isOutput() && clip.isConnected())
        {
            const attribute::ClipImage& linkClip = clip.getConnectedClip();
            if(clip.isSupportedComponent(mostChromaticComponents))
                clip.setComponentsStringIfNotModifiedByPlugin(linkClip.getComponentsString());
        }
    }
    if(outputClip.isSupportedComponent(mostChromaticComponents))
        outputClip.setComponentsStringIfNotModifiedByPlugin(mostChromaticComponents);
}

/// @todo multiple PAR
void ImageEffectNode::initInputClipsPixelAspectRatio()
{
    std::set<double> inputPARs;
    //	if( supportsMultipleClipPARs() )
    {
        for(ClipImageMap::iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
        {
            attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(it->second));
            TUTTLE_LOG_INFO("[Clip] " << clip.getName());
            if(!clip.isOutput() && clip.isConnected())
            {
                const attribute::ClipImage& linkClip = clip.getConnectedClip();
                const double par = linkClip.getPixelAspectRatio();
                TUTTLE_LOG_INFO("[Clip] " << linkClip.getName() << ", pixel aspect ratio = " << par);
                clip.setPixelAspectRatio(par, ofx::property::eModifiedByHost);
                inputPARs.insert(par);
            }
        }
    }
    //	else
    //	{
    //		// @todo The plugin doesn't support PAR, the host should do the conversions!
    //		// http://openfx.sourceforge.net/Documentation/1.3/ofxProgrammingReference.html#ImageEffectsPixelAspectRatios
    //		// If a plugin does not accept clips of differing PARs, then the host must resample all images fed to that effect
    //to agree with the output's PAR.
    //		// If a plugin does accept clips of differing PARs, it will need to specify the output clip's PAR in the
    //kOfxImageEffectActionGetClipPreferences action.
    //
    //		// Convert images here ? Or introduce convert nodes into the ProcessGraph?
    //		BOOST_ASSERT(false);
    //	}

    // Not supported yet. So fail in debug,
    // and process with a wrong pixel aspect ratio in release.
    TUTTLE_LOG_INFO("[Clip] support Multiple clip PAR = " << supportsMultipleClipPARs());
    TUTTLE_LOG_INFO("[Clip] number of clips = " << getNbClips());
    BOOST_ASSERT(inputPARs.size() <= 1 || supportsMultipleClipPARs() || getNbClips() <= 2);
}

void ImageEffectNode::initInputClipsFps()
{
    for(ClipImageMap::iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
    {
        attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(it->second));
        if(!clip.isOutput() && clip.isConnected())
        {
            const attribute::ClipImage& linkClip = clip.getConnectedClip();
            clip.setFrameRate(linkClip.getFrameRate());
        }
    }
}

void ImageEffectNode::initFps()
{
    attribute::ClipImage& outputClip = dynamic_cast<attribute::ClipImage&>(getOutputClip());
    outputClip.setFrameRate(getOutputFrameRate());
}

void ImageEffectNode::initPixelAspectRatio()
{
    attribute::ClipImage& outputClip = dynamic_cast<attribute::ClipImage&>(getOutputClip());
    outputClip.setPixelAspectRatio(getOutputPixelAspectRatio(), ofx::property::eModifiedByHost);
}

void ImageEffectNode::maximizeBitDepthFromReadsToWrites()
{
    std::string biggestBitDepth = kOfxBitDepthNone;
    attribute::ClipImage& outputClip = dynamic_cast<attribute::ClipImage&>(getOutputClip());
    bool inputClipsFound = false;

    // init variables
    for(ClipImageMap::iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
    {
        attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(it->second));
        if(!clip.isOutput() && clip.isConnected()) // filter for clip.isMask() and clip.isOptional() ?
        {
            inputClipsFound = true;
            const attribute::ClipImage& linkClip = clip.getConnectedClip();
            biggestBitDepth = ofx::imageEffect::findDeepestBitDepth(linkClip.getBitDepthString(), biggestBitDepth);
        }
    }
    const std::string validBitDepth = this->bestSupportedBitDepth(biggestBitDepth);

    // bit depth
    if(supportsMultipleClipDepths())
    {
        // check if we support the bit depth of each input
        // and fill input clip with connected clips bit depth
        for(ClipImageMap::iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
        {
            attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(it->second));
            if(!clip.isOutput() && clip.isConnected())
            {
                const attribute::ClipImage& linkClip = clip.getConnectedClip();
                const std::string& linkClipBitDepth = linkClip.getBitDepthString();
                if(this->isSupportedBitDepth(linkClipBitDepth))
                {
                    clip.setBitDepthStringIfUpperAndNotModifiedByPlugin(linkClipBitDepth);
                }
            }
        }
    }
    else // multiple clip depth not supported (standard case)
    {
        if(inputClipsFound &&                 // if we have an input clip
           validBitDepth == kOfxBitDepthNone) // if we didn't found a valid bit depth value
        {
            BOOST_THROW_EXCEPTION(exception::Logic() << exception::user("Pixel depth " + biggestBitDepth +
                                                                        " not supported on plugin : " + getName()));
        }
        for(ClipImageMap::iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
        {
            attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(it->second));
            if(!clip.isOutput() && clip.isConnected())
            {
                const attribute::ClipImage& linkClip = clip.getConnectedClip();
                if((linkClip.getNode().getNodeType() == INode::eNodeTypeImageEffect &&
                    linkClip.getNode().asImageEffectNode().isSupportedBitDepth(validBitDepth)) ||
                   linkClip.getNode().getNodeType() == INode::eNodeTypeBuffer)
                {
                    clip.setBitDepthStringIfUpperAndNotModifiedByPlugin(validBitDepth);
                }
            }
        }
    }
    outputClip.setBitDepthStringIfUpperAndNotModifiedByPlugin(validBitDepth);
}

void ImageEffectNode::maximizeBitDepthFromWritesToReads()
{
    // TUTTLE_LOG_INFO( "maximizeBitDepthFromWritesToReads: " << getName() );
    if(!supportsMultipleClipDepths())
    {
        attribute::ClipImage& outputClip = dynamic_cast<attribute::ClipImage&>(getOutputClip());
        const std::string& outputClipBitDepthStr = outputClip.getBitDepthString();
        for(ClipImageMap::iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
        {
            attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(it->second));
            if(!clip.isOutput() && clip.isConnected())
            {
                /// @todo tuttle: what is the best way to access another node ?
                /// through the graph ? through a graph inside ProcessOptions ?
                /*const */ attribute::ClipImage& linkClip = clip.getConnectedClip();

                // TUTTLE_LOG_INFO( clip.getFullName() << "(" << clip.getBitDepth() << ")" << "-->" << linkClip.getFullName()
                // << "(" << linkClip.getBitDepth() << ")" );
                if(linkClip.getNode().getNodeType() == INode::eNodeTypeImageEffect &&
                   linkClip.getNode().asImageEffectNode().isSupportedBitDepth(
                       outputClipBitDepthStr)) // need to be supported by the other node
                {
                    if(linkClip.getNode().asImageEffectNode().supportsMultipleClipDepths()) /// @todo tuttle: is this test
                                                                                            /// correct in all cases?
                    {
                        linkClip.setBitDepthStringIfUpper(outputClipBitDepthStr);
                    }
                    else
                    {
                        linkClip.setBitDepthStringIfUpperAndNotModifiedByPlugin(outputClipBitDepthStr);
                    }
                }
                // TUTTLE_LOG_INFO( clip.getFullName() << "(" << clip.getBitDepth() << ")" << "-->" << linkClip.getFullName()
                // << "(" << linkClip.getBitDepth() << ")" );
            }
            // else
            //{
            //	TUTTLE_LOG_INFO( clip.getFullName() << "(" << clip.getBitDepth() << ")" << ", unconnected ? " <<
            //clip.isConnected() << ", output ? " << clip.isOutput() );
            //}
        }
    }
}

void ImageEffectNode::coutBitDepthConnections() const
{
#ifdef TUTTLE_DEBUG
    // validation
    for(ClipImageMap::const_iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
    {
        const attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(it->second));

        // const ofx::property::String& propPixelDepth       = clip.getProperties().fetchStringProperty(
        // kOfxImageEffectPropPixelDepth );
        // const ofx::property::String& propComponent        = clip.getProperties().fetchStringProperty(
        // kOfxImageEffectPropComponents );
        // const ofx::property::Double& propPixelAspectRatio = clip.getProperties().fetchDoubleProperty(
        // kOfxImagePropPixelAspectRatio );
        /*
        TUTTLE_LOG_INFO( "-- " << "clip: " << " = " << clip.getFullName() );
        TUTTLE_LOG_INFO( "-- " << kOfxImageEffectPropPixelDepth << " = " << propPixelDepth.getValue()
                                                   << " : " << ( propPixelDepth.getModifiedBy() ==
        ofx::property::eModifiedByPlugin ? "(plugin)" : "(host)" ) );
        TUTTLE_LOG_INFO( "-- " << kOfxImageEffectPropComponents << " = " << propComponent.getValue()
                                                   << " : " << ( propComponent.getModifiedBy() ==
        ofx::property::eModifiedByPlugin ? "(plugin)" : "(host)" ) );
        TUTTLE_LOG_INFO( "-- " << kOfxImagePropPixelAspectRatio << " = " << propPixelAspectRatio.getValue()
                                                   << " : " << ( propPixelAspectRatio.getModifiedBy() ==
        ofx::property::eModifiedByPlugin ? "(plugin)" : "(host)" ) );
        */
        if(!clip.isOutput() && clip.isConnected())
        {
            const attribute::ClipImage& linkClip = clip.getConnectedClip();
            TUTTLE_LOG_INFO("[Bit Depth Connection] Connection between "
                            << clip.getFullName() << " (" << clip.getBitDepth() << " bytes)"
                            << " => " << linkClip.getFullName() << " (" << linkClip.getBitDepth() << " bytes).");
        }
    }
#endif
}

void ImageEffectNode::validInputClipsConnections() const
{
    // validation
    for(ClipImageMap::const_iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
    {
        const attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(it->second));
        if(!clip.isOutput() && clip.isConnected())
        {
            const attribute::ClipImage& linkClip = clip.getConnectedClip();
            if(clip.getComponents() != linkClip.getComponents())
            {
                BOOST_THROW_EXCEPTION(exception::Logic()
                                      << exception::dev() + "Error in graph components propagation.\n"
                                                            "Connection \"" +
                                             linkClip.getFullName() + "\" (" + linkClip.getComponentsString() + ")" +
                                             " => \"" + clip.getFullName() + "\" (" + clip.getComponentsString() + ")."
                                      << exception::pluginName(getName())
                                      << exception::pluginIdentifier(getPlugin().getIdentifier()));
            }
            if(clip.getBitDepth() != linkClip.getBitDepth())
            {
                BOOST_THROW_EXCEPTION(exception::Logic()
                                      << exception::dev() + "Error in graph bit depth propagation.\n"
                                                            "Connection \"" +
                                             linkClip.getFullName() + "\" (" + linkClip.getBitDepth() + " bytes)" +
                                             " => \"" + clip.getFullName() + "\" (" + clip.getBitDepth() + " bytes)."
                                      << exception::pluginName(getName())
                                      << exception::pluginIdentifier(getPlugin().getIdentifier()));
            }
        }
    }
}

OfxRangeD ImageEffectNode::getDefaultTimeDomain() const
{
    // TUTTLE_LOG_INFO( "- ImageEffectNode::getDefaultTimeDomain: " << getName() );
    OfxRangeD range;
    range.min = kOfxFlagInfiniteMin;
    range.max = kOfxFlagInfiniteMax;
    // if no answer, compute it from input clips
    bool first = true;
    for(ClipImageMap::const_iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
    {
        const attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(it->second));
        if(!clip.isOutput() && clip.isConnected())
        {
            const attribute::ClipImage& linkClip = clip.getConnectedClip();
            const OfxRangeD clipRange = linkClip.getNode().getTimeDomain();
            if(first)
            {
                first = false;
                range = clipRange;
            }
            else
            {
                // maybe better to use intersection instead of union
                range.min = std::min(range.min, clipRange.min);
                range.max = std::max(range.max, clipRange.max);
            }
        }
    }
    return range;
}

OfxRangeD ImageEffectNode::computeTimeDomain()
{
    // Copy connected clips frameRange into each input clips
    for(ClipImageMap::iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
    {
        attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(it->second));
        if(!clip.isOutput() && clip.isConnected())
        {
            const attribute::ClipImage& linkClip = clip.getConnectedClip();
            const OfxRangeD clipRange = linkClip.getFrameRange();
            clip.setFrameRange(clipRange.min, clipRange.max);
        }
    }
    TUTTLE_LOG_INFO("[Time domain] getTimeDomain " << quotes(getName()) << " computed by the host.");
    OfxRangeD defaultRange = getDefaultTimeDomain();
    OfxRangeD range = defaultRange;

    // ask to the plugin
    if(getTimeDomainAction(range))
    {
        TUTTLE_LOG_INFO("[Time domain] getTimeDomain " << quotes(getName()) << " computed by the plugin.");
    }
    else
    {
        range = defaultRange;
    }
    attribute::ClipImage* clip = dynamic_cast<attribute::ClipImage*>(_clipImages[kOfxImageEffectOutputClipName]);
    if(clip)
    {
        clip->setFrameRange(range.min, range.max);
        clip->setUnmappedFrameRange(range.min, range.max);
    }
    return range;
}

void ImageEffectNode::setup1()
{
    checkClipsConnected();

    initInputClipsFps();
    initInputClipsPixelAspectRatio();

    getClipPreferencesAction();

    initComponents();
    initPixelAspectRatio();
    initFps();

    maximizeBitDepthFromReadsToWrites();
}

void ImageEffectNode::setup2_reverse()
{
    maximizeBitDepthFromWritesToReads();
}

void ImageEffectNode::setup3()
{
    maximizeBitDepthFromReadsToWrites();
    // coutBitDepthConnections();
    validInputClipsConnections();
}

void ImageEffectNode::beginSequence(graph::ProcessVertexData& vData)
{
    // TUTTLE_LOG_INFO( "begin: " << getName() );
    beginSequenceRenderAction(vData._renderTimeRange.min, vData._renderTimeRange.max, vData._step, vData._interactive,
                              vData._renderScale);
}

void ImageEffectNode::preProcess1(graph::ProcessVertexAtTimeData& vData)
{
    TUTTLE_LOG_INFO("[Pre Process 1] " << getName() << " at time: " << vData._time);
    // setCurrentTime( vData._time );

    OfxRectD rod;
    getRegionOfDefinitionAction(vData._time, vData._nodeData->_renderScale, rod);
    //	TUTTLE_LOG_VAR3( TUTTLE_INFO, this->getName(), vData._time, rod );
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, &getData(vData._time) );
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, &vData );
    vData._apiImageEffect._renderRoD = rod;
    vData._apiImageEffect._renderRoI = rod; ///< @todo tuttle: tile supports

    TUTTLE_LOG_INFO("[Pre Process 1] rod: x1:" << rod.x1 << " y1:" << rod.y1 << " x2:" << rod.x2 << " y2:" << rod.y2);
}

void ImageEffectNode::preProcess2_reverse(graph::ProcessVertexAtTimeData& vData)
{
    //	TUTTLE_LOG_INFO( "preProcess2_finish: " << getName() << " at time: " << vData._time );

    getRegionOfInterestAction(vData._time, vData._nodeData->_renderScale, vData._apiImageEffect._renderRoI,
                              vData._apiImageEffect._inputsRoI);
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, vData._renderRoD );
    //	TUTTLE_LOG_VAR( TUTTLE_INFO, vData._renderRoI );
}

bool ImageEffectNode::isIdentity(const graph::ProcessVertexAtTimeData& vData, std::string& clip, OfxTime& time) const
{
    time = vData._time;
    double par = this->getOutputClip().getPixelAspectRatio();
    if(par == 0.0)
        par = 1.0;
    OfxRectI renderWindow;
    renderWindow.x1 = boost::numeric_cast<int>(std::floor(vData._apiImageEffect._renderRoI.x1 / par));
    renderWindow.x2 = boost::numeric_cast<int>(std::ceil(vData._apiImageEffect._renderRoI.x2 / par));
    renderWindow.y1 = boost::numeric_cast<int>(std::floor(vData._apiImageEffect._renderRoI.y1));
    renderWindow.y2 = boost::numeric_cast<int>(std::ceil(vData._apiImageEffect._renderRoI.y2));
    return isIdentityAction(time, vData._apiImageEffect._field, renderWindow, vData._nodeData->_renderScale, clip);
}

void ImageEffectNode::preProcess_infos(const graph::ProcessVertexAtTimeData& vData, const OfxTime time,
                                       graph::ProcessVertexAtTimeInfo& nodeInfos) const
{
    //	TUTTLE_LOG_INFO( "preProcess_infos: " << getName() );
    const OfxRectD rod = vData._apiImageEffect._renderRoD;
    const std::size_t bitDepth = this->getOutputClip().getBitDepth(); // value in bytes
    const std::size_t nbComponents = getOutputClip().getNbComponents();
    nodeInfos._memory = std::ceil((rod.x2 - rod.x1) * (rod.y2 - rod.y1) * nbComponents * bitDepth);
}

void ImageEffectNode::process(graph::ProcessVertexAtTimeData& vData)
{
    try
    {
        memory::IMemoryCache& memoryCache = vData._nodeData->getInternMemoryCache();
        // keep the hand on all needed datas during the process function
        std::list<memory::CACHE_ELEMENT> allNeededDatas;

        double par = this->getOutputClip().getPixelAspectRatio();
        if(par == 0.0)
            par = 1.0;
        const OfxRectI renderWindow = {boost::numeric_cast<int>(std::floor(vData._apiImageEffect._renderRoI.x1 / par)),
                                       boost::numeric_cast<int>(std::floor(vData._apiImageEffect._renderRoI.y1)),
                                       boost::numeric_cast<int>(std::ceil(vData._apiImageEffect._renderRoI.x2 / par)),
                                       boost::numeric_cast<int>(std::ceil(vData._apiImageEffect._renderRoI.y2))};
        //		TUTTLE_LOG_VAR( TUTTLE_INFO, roi );

        //		INode::ClipTimesSetMap timesSetMap = this->getFramesNeeded( vData._time );

        // acquire needed clip images
        /*
        TUTTLE_LOG_INFO( "acquire needed input clip images" );
        TUTTLE_LOG_VAR( TUTTLE_INFO, vData._inEdges.size() );
        TUTTLE_LOG_VAR( TUTTLE_INFO, vData._outEdges.size() );
        BOOST_FOREACH( const graph::ProcessEdgeAtTime* o, vData._outEdges )
        {
                TUTTLE_LOG_VAR( TUTTLE_INFO, o );
                TUTTLE_LOG_VAR( TUTTLE_INFO, o->getInTime() );
                TUTTLE_LOG_VAR( TUTTLE_INFO, o->getInAttrName() );
        }
        BOOST_FOREACH( const graph::ProcessEdgeAtTime* i, vData._inEdges )
        {
                TUTTLE_LOG_VAR( TUTTLE_INFO, i );
                TUTTLE_LOG_VAR( TUTTLE_INFO, i->getInTime() );
                TUTTLE_LOG_VAR( TUTTLE_INFO, i->getInAttrName() );
        }
        */
        TUTTLE_LOG_INFO("[Node Process] Acquire needed input clips images");
        BOOST_FOREACH(const graph::ProcessVertexAtTimeData::ProcessEdgeAtTimeByClipName::value_type& inEdgePair,
                      vData._inEdges)
        {
            const graph::ProcessEdgeAtTime* inEdge = inEdgePair.second;
            // TUTTLE_LOG_VAR( TUTTLE_INFO, i );
            // TUTTLE_LOG_VAR( TUTTLE_INFO, i->getInTime() );
            // TUTTLE_LOG_VAR( TUTTLE_INFO, i->getInAttrName() );
            attribute::ClipImage& clip = getClip(inEdge->getInAttrName());
            const OfxTime outTime = inEdge->getOutTime();

            TUTTLE_LOG_INFO("[Node Process] out: " << inEdge->getOut() << " -> in " << inEdge->getIn());
            memory::CACHE_ELEMENT imageCache(memoryCache.get(clip.getClipIdentifier(), outTime));
            if(imageCache.get() == NULL)
            {
                BOOST_THROW_EXCEPTION(exception::Memory() << exception::dev() + "Input attribute " +
                                                                 quotes(clip.getFullName()) + " at time " + vData._time +
                                                                 " not in memory cache (identifier:" +
                                                                 quotes(clip.getClipIdentifier()) + ").");
            }
            allNeededDatas.push_back(imageCache);
        }

        TUTTLE_LOG_INFO("[Node Process] Acquire needed output clip images");
        BOOST_FOREACH(ClipImageMap::value_type& i, _clipImages)
        {
            attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(i.second));
            if(clip.isOutput())
            {
                TUTTLE_LOG_INFO("[Node Process] " << vData._apiImageEffect._renderRoI);
                memory::CACHE_ELEMENT imageCache(new attribute::Image(clip, vData._time, vData._apiImageEffect._renderRoI,
                                                                      attribute::Image::eImageOrientationFromBottomToTop,
                                                                      0));
                imageCache->setPoolData(core().getMemoryPool().allocate(imageCache->getMemorySize()));
                memoryCache.put(clip.getClipIdentifier(), vData._time, imageCache);

                allNeededDatas.push_back(imageCache);
            }
        }

        TUTTLE_LOG_TRACE("[Node Process] Plugin Render Action");

        renderAction(vData._time, vData._apiImageEffect._field, renderWindow, vData._nodeData->_renderScale);

        TUTTLE_LOG_TRACE("[Node Process] Plugin Render Action - End");

        debugOutputImage(vData._time);

        // release input images
        BOOST_FOREACH(const graph::ProcessVertexAtTimeData::ProcessEdgeAtTimeByClipName::value_type& inEdgePair,
                      vData._inEdges)
        {
            const graph::ProcessEdgeAtTime* inEdge = inEdgePair.second;
            attribute::ClipImage& clip = getClip(inEdge->getInAttrName());
            const OfxTime outTime = inEdge->getOutTime();

            // TUTTLE_LOG_VAR2( TUTTLE_INFO, clip.getClipIdentifier(), outTime );
            // TUTTLE_LOG_VAR2( TUTTLE_INFO, inEdge->getOut(), inEdge->getIn() );
            // TUTTLE_LOG_VAR2( TUTTLE_INFO, clip.getClipIdentifier(), clip.getFullName() );

            memory::CACHE_ELEMENT imageCache = memoryCache.get(clip.getClipIdentifier(), outTime);
            if(imageCache.get() == NULL)
            {
                BOOST_THROW_EXCEPTION(exception::Memory() << exception::dev() + "Clip " + quotes(clip.getFullName()) +
                                                                 " not in memory cache (identifier: " +
                                                                 quotes(clip.getClipIdentifier()) + ", time: " + outTime +
                                                                 ").");
            }
            TUTTLE_LOG_TRACE("[ImageEffectNode] releaseReference: " << imageCache->getFullName());
            // TODO: use RAII technique for add/releaseReference...
            imageCache->releaseReference(ofx::imageEffect::OfxhImage::eReferenceOwnerHost);
        }

        // declare future usages of the output
        BOOST_FOREACH(ClipImageMap::value_type& item, _clipImages)
        {
            attribute::ClipImage& clip = dynamic_cast<attribute::ClipImage&>(*(item.second));
            if(!clip.isOutput() && !clip.isConnected())
                continue;

            if(clip.isOutput())
            {
                memory::CACHE_ELEMENT imageCache = memoryCache.get(clip.getClipIdentifier(), vData._time);
                if(imageCache.get() == NULL)
                {
                    BOOST_THROW_EXCEPTION(exception::Memory() << exception::dev() + "Clip " + quotes(clip.getFullName()) +
                                                                     " not in memory cache (identifier:" +
                                                                     quotes(clip.getClipIdentifier()) + ").");
                }
                const std::size_t realOutDegree =
                    vData._outDegree - vData._isFinalNode; // final nodes have a connection to the fake output node.
                TUTTLE_LOG_INFO("[Node Process] Declare future usages: " << clip.getClipIdentifier()
                                                                         << ", add reference: " << realOutDegree);
                if(realOutDegree > 0)
                {
                    TUTTLE_LOG_TRACE("[ImageEffectNode] addReference: " << imageCache->getFullName()
                                                                        << ", degree=" << realOutDegree);
                    // TODO: use RAII technique for add/releaseReference...
                    //       to properly declare image unused when an error occured
                    //       during the computation.
                    // Add a reference on this node for each future usages
                    imageCache->addReference(ofx::imageEffect::OfxhImage::eReferenceOwnerHost, realOutDegree);
                }
            }
        }
    }
    catch(boost::exception& e)
    {
        e << exception::time(vData._time) << exception::pluginIdentifier(this->getPlugin().getIdentifier())
          << exception::nodeName(this->getName());
        throw;
    }
}

void ImageEffectNode::postProcess(graph::ProcessVertexAtTimeData& vData)
{
    //	TUTTLE_LOG_INFO( "postProcess: " << getName() );
}

void ImageEffectNode::endSequence(graph::ProcessVertexData& vData)
{
    //	TUTTLE_LOG_INFO( "end: " << getName() );
    endSequenceRenderAction(vData._renderTimeRange.min, vData._renderTimeRange.max, vData._step, vData._interactive,
                            vData._renderScale);
}

std::ostream& ImageEffectNode::print(std::ostream& os) const
{
    const ImageEffectNode& v = *this;
    os << "________________________________________________________________________________" << std::endl;
    os << "Plug-in:" << v.getLabel() << std::endl;
    os << "Description:" << v.getLongLabel() << std::endl;
    os << "Context:" << v._context << std::endl;
    os << "Clips:" << std::endl;
    for(ImageEffectNode::ClipImageMap::const_iterator it = v._clipImages.begin(), itEnd = v._clipImages.end(); it != itEnd;
        ++it)
    {
        os << "  * " << it->second->getName() << std::endl;
    }
    os << "Params:" << std::endl;
    for(ImageEffectNode::ParamVector::const_iterator it = v._paramVector.begin(), itEnd = v._paramVector.end(); it != itEnd;
        ++it)
    {
        os << "  * " << it->getName() << " (" << it->getLabel() << "): " << it->displayValues(os) << std::endl;
    }
    os << "________________________________________________________________________________" << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const ImageEffectNode& v)
{
    return v.print(os);
}

void ImageEffectNode::debugOutputImage(const OfxTime time) const
{
#if(TUTTLE_PNG_EXPORT_BETWEEN_NODES)
    for(ClipImageMap::const_iterator it = _clipImages.begin(); it != _clipImages.end(); ++it)
    {
        const attribute::ClipImage& clip = dynamic_cast<const attribute::ClipImage&>(*(it->second));
        if(clip.isOutput())
        {
            boost::shared_ptr<attribute::Image> image =
                clip.getNode().getData().getInternMemoryCache().get(this->getName() + "." kOfxOutputAttributeName, time);
            image->debugSaveAsPng(boost::lexical_cast<std::string>(time) + "_" + this->getName() + ".png");
        }
    }
#endif
}
}
}
