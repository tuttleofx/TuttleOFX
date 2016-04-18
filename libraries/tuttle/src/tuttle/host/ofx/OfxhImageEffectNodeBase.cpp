#include "OfxhImageEffectNodeBase.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace imageEffect
{

//
// Base
//
OfxhImageEffectNodeBase::OfxhImageEffectNodeBase(const property::OfxhSet& set)
    : _properties(set)
{
}

OfxhImageEffectNodeBase::OfxhImageEffectNodeBase(const property::OfxhPropSpec* propSpec)
    : _properties(propSpec)
{
}

OfxhImageEffectNodeBase::~OfxhImageEffectNodeBase()
{
}

bool OfxhImageEffectNodeBase::operator==(const OfxhImageEffectNodeBase& other) const
{
    return _properties == other._properties;
}

/**
 * obtain a handle on this for passing to the C api
 */
OfxImageEffectHandle OfxhImageEffectNodeBase::getHandle() const
{
    return (OfxImageEffectHandle) this;
}

/// name of the imageEffect
const std::string& OfxhImageEffectNodeBase::getShortLabel() const
{
    const std::string& s = _properties.getStringProperty(kOfxPropShortLabel);

    if(s == "")
    {
        const std::string& s2 = getLabel();
        if(s2 == "")
        {
            return getName();
        }
    }
    return s;
}

/// name of the imageEffect
const std::string& OfxhImageEffectNodeBase::getLabel() const
{
    const std::string& s = _properties.getStringProperty(kOfxPropLabel);

    if(s == "")
    {
        return getName();
    }
    return s;
}

/// name of the imageEffect
const std::string& OfxhImageEffectNodeBase::getName() const
{
    return _properties.getStringProperty(kOfxPropName);
}

/// name of the imageEffect
void OfxhImageEffectNodeBase::setName(const std::string& name)
{
    _properties.setStringProperty(kOfxPropName, name);
}

const std::string& OfxhImageEffectNodeBase::getLongLabel() const
{
    const std::string& s = _properties.getStringProperty(kOfxPropLongLabel);

    if(s == "")
    {
        const std::string& s2 = getLabel();
        if(s2 == "")
        {
            return getName();
        }
    }
    return s;
}

/// is the given context supported

bool OfxhImageEffectNodeBase::isContextSupported(const std::string& s) const
{
    return _properties.findStringPropValueIndex(kOfxImageEffectPropSupportedContexts, s) != -1;
}

/// what is the name of the group the plug-in belongs to

const std::string& OfxhImageEffectNodeBase::getPluginGrouping() const
{
    return _properties.getStringProperty(kOfxImageEffectPluginPropGrouping);
}

/// is the effect single instance

bool OfxhImageEffectNodeBase::isSingleInstance() const
{
    return _properties.getIntProperty(kOfxImageEffectPluginPropSingleInstance) != 0;
}

/// what is the thread safety on this effect

const std::string& OfxhImageEffectNodeBase::getRenderThreadSafety() const
{
    return _properties.getStringProperty(kOfxImageEffectPluginRenderThreadSafety);
}

/// should the host attempt to managed multi-threaded rendering if it can
/// via tiling or some such

bool OfxhImageEffectNodeBase::getHostFrameThreading() const
{
    return _properties.getIntProperty(kOfxImageEffectPluginPropHostFrameThreading) != 0;
}

/// get the overlay interact main entry if it exists

OfxPluginEntryPoint* OfxhImageEffectNodeBase::getOverlayInteractMainEntry() const
{
    return (OfxPluginEntryPoint*)(_properties.getPointerProperty(kOfxImageEffectPluginPropOverlayInteractV1));
}

/// does the effect support images of differing sizes

bool OfxhImageEffectNodeBase::supportsMultiResolution() const
{
    return _properties.getIntProperty(kOfxImageEffectPropSupportsMultiResolution) != 0;
}

/// does the effect support tiled rendering

bool OfxhImageEffectNodeBase::supportsTiles() const
{
    return _properties.getIntProperty(kOfxImageEffectPropSupportsTiles) != 0;
}

/// does this effect need random temporal access

bool OfxhImageEffectNodeBase::temporalAccess() const
{
    return _properties.getIntProperty(kOfxImageEffectPropTemporalClipAccess) != 0;
}

/// is the given RGBA/A pixel depth supported by the effect

bool OfxhImageEffectNodeBase::isBitDepthSupported(const std::string& s) const
{
    return _properties.findStringPropValueIndex(kOfxImageEffectPropSupportedPixelDepths, s) != -1;
}

/// when field rendering, does the effect need to be called
/// twice to render a frame in all Base::circumstances (with different fields)

bool OfxhImageEffectNodeBase::fieldRenderTwiceAlways() const
{
    return _properties.getIntProperty(kOfxImageEffectPluginPropFieldRenderTwiceAlways) != 0;
}

/// does the effect support multiple clip depths

bool OfxhImageEffectNodeBase::supportsMultipleClipDepths() const
{
    return _properties.getIntProperty(kOfxImageEffectPropSupportsMultipleClipDepths) != 0;
}

/// does the effect support multiple clip pixel aspect ratios

bool OfxhImageEffectNodeBase::supportsMultipleClipPARs() const
{
    return _properties.getIntProperty(kOfxImageEffectPropSupportsMultipleClipPARs) != 0;
}

/// does changing the named param re-tigger a clip preferences action

bool OfxhImageEffectNodeBase::isClipPreferencesSlaveParam(const std::string& s) const
{
    return _properties.findStringPropValueIndex(kOfxImageEffectPropClipPreferencesSlaveParam, s) != -1;
}
}
}
}
}
