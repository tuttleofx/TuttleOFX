#ifndef _TUTTLE_HOST_OFX_IMAGEEFFECTNODE_DESCRIPTOR_HPP_
#define _TUTTLE_HOST_OFX_IMAGEEFFECTNODE_DESCRIPTOR_HPP_

#include "OfxhImageEffectNodeBase.hpp"
#include "attribute/OfxhClipImageDescriptor.hpp"
#include "attribute/OfxhParamDescriptor.hpp"
#include "attribute/OfxhParamSetDescriptor.hpp"
#include "OfxhInteract.hpp"

#include <boost/ptr_container/serialize_ptr_vector.hpp>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace imageEffect
{

/**
 * an image effect plugin descriptor
 */
class OfxhImageEffectNodeDescriptor : public OfxhImageEffectNodeBase,
                                      public attribute::OfxhParamSetDescriptor,
                                      private boost::noncopyable
{
public:
    typedef OfxhImageEffectNodeDescriptor This;
    typedef std::map<std::string, attribute::OfxhClipImageDescriptor*> ClipImageDescriptorMap;
    typedef boost::ptr_vector<attribute::OfxhClipImageDescriptor> ClipImageDescriptorVector;

protected:
    OfxhPlugin* _plugin;                     ///< the plugin I belong to
    ClipImageDescriptorMap _clips;           ///< clips descriptors by name
    ClipImageDescriptorVector _clipsByOrder; ///< clip descriptors in order of declaration
    mutable interact::OfxhInteractDescriptor
        _overlayDescriptor; ///< descriptor to use for overlays, it has delayed description @todo tuttle: remove mutable

private:
    // private CC
    OfxhImageEffectNodeDescriptor(const OfxhImageEffectNodeDescriptor& other)
        : OfxhImageEffectNodeBase(other._properties)
        , _plugin(other._plugin)
    {
    }

    OfxhImageEffectNodeDescriptor();

public:
    /// used to construct the global description
    OfxhImageEffectNodeDescriptor(OfxhPlugin& plug);

    /// used to construct a context description, 'other' is the main context
    OfxhImageEffectNodeDescriptor(const OfxhImageEffectNodeDescriptor& rootContext, OfxhPlugin& plug);

    /// used to construct populate the cache
    OfxhImageEffectNodeDescriptor(const std::string& bundlePath, OfxhPlugin& plug);

    /// dtor
    virtual ~OfxhImageEffectNodeDescriptor();

    bool operator==(const This& other) const;
    bool operator!=(const This& other) const { return !This::operator==(other); }

    /// implemented for ParamDescriptorSet
    property::OfxhSet& getParamSetProps() { return _properties; }

    /// get the plugin I belong to
    OfxhPlugin& getPlugin() const { return *_plugin; }

    /// create a new clip and add this to the clip map
    virtual attribute::OfxhClipImageDescriptor* defineClip(const std::string& name);

    /// get the clips
    const ClipImageDescriptorMap& getClips() const { return _clips; }

    /// add a new clip
    void addClip(const std::string& name, attribute::OfxhClipImageDescriptor* clip);

    /// get the clips in order of construction
    const ClipImageDescriptorVector& getClipsByOrder() const { return _clipsByOrder; }

    /// get the clips in order of construction
    ClipImageDescriptorVector& getClipsByOrder() { return _clipsByOrder; }

    /**
     * @todo tuttle some modifs here... doc needs updates...
     * Get the interact description, this will also call describe on the interact
     * This will return NULL if there is not main entry point or if the description failed
     * otherwise it will return the described overlay
     */
    const interact::OfxhInteractDescriptor& getOverlayDescriptor() const { return _overlayDescriptor; }

    void initOverlayDescriptor(int bitDepthPerComponent = 8, bool hasAlpha = false);

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(OfxhImageEffectNodeBase);
        ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(OfxhParamSetDescriptor);
        ar& BOOST_SERIALIZATION_NVP(_clipsByOrder);
    }
};
}
}
}
}

#endif
