#ifndef _TUTTLE_HOST_OFX_IMAGEEFFECTNODE_BASE_HPP_
#define _TUTTLE_HOST_OFX_IMAGEEFFECTNODE_BASE_HPP_

#include "OfxhIObject.hpp"
#include "property/OfxhSet.hpp"

#include <boost/serialization/extended_type_info.hpp>
#include <boost/serialization/export.hpp>

namespace tuttle
{
namespace host
{
namespace ofx
{

// forward declare
class OfxhPlugin;

namespace attribute
{
class OfxhClipDescriptor;
class OfxhClip;
}

namespace imageEffect
{

////////////////////////////////////////////////////////////////////////////////
/**
 * base class to both effect descriptors and instances
 */
class OfxhImageEffectNodeBase : virtual public OfxhIObject
{
protected:
    property::OfxhSet _properties;

public:
    OfxhImageEffectNodeBase(const property::OfxhSet& set);
    OfxhImageEffectNodeBase(const property::OfxhPropSpec* propSpec);
    virtual ~OfxhImageEffectNodeBase();

    bool operator==(const OfxhImageEffectNodeBase& other) const;
    bool operator!=(const OfxhImageEffectNodeBase& other) const { return !operator==(other); }

    /// is my magic number valid?
    virtual bool verifyMagic() { return true; }

    /// obtain a handle on this for passing to the C api
    OfxImageEffectHandle getHandle() const;

    const property::OfxhSet& getProperties() const { return _properties; }
    property::OfxhSet& getEditableProperties() { return _properties; }

    /// name of the Node
    const std::string& getShortLabel() const;

    /// name of the Node
    const std::string& getLabel() const;

    /// name of the Node
    const std::string& getName() const;

    /// name of the Node
    void setName(const std::string& name);

    /// Description of the Node
    const std::string& getLongLabel() const;

    /// is the given context supported
    bool isContextSupported(const std::string& s) const;

    /// what is the name of the group the plug-in belongs to
    const std::string& getPluginGrouping() const;

    /// is the effect single instance
    bool isSingleInstance() const;

    /// what is the thread safety on this effect
    const std::string& getRenderThreadSafety() const;

    /// should the host attempt to managed multi-threaded rendering if it can
    /// via tiling or some such
    bool getHostFrameThreading() const;

#ifndef SWIG
    /// get the overlay interact main entry if it exists
    OfxPluginEntryPoint* getOverlayInteractMainEntry() const;
#endif

    /// does the effect support images of differing sizes
    bool supportsMultiResolution() const;

    /// does the effect support tiled rendering
    bool supportsTiles() const;

    /// does this effect need random temporal access
    bool temporalAccess() const;

    /// is the given bit depth supported by the effect
    bool isBitDepthSupported(const std::string& s) const;

    /// when field rendering, does the effect need to be called
    /// twice to render a frame in all circumstances (with different fields)
    bool fieldRenderTwiceAlways() const;

    /// does the effect support multiple clip depths
    bool supportsMultipleClipDepths() const;

    /// does the effect support multiple clip pixel aspect ratios
    bool supportsMultipleClipPARs() const;

    /// does changing the named param re-tigger a clip preferences action
    bool isClipPreferencesSlaveParam(const std::string& s) const;

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& BOOST_SERIALIZATION_NVP(_properties);
    }
};
}
}
}
}

#endif
