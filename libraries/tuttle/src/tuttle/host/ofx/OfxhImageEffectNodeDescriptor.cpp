#include "OfxhImageEffectNodeDescriptor.hpp"
#include "OfxhPlugin.hpp"
#include "OfxhPluginBinary.hpp"

#include <tuttle/host/Core.hpp> /// @todo tuttle: please remove this ! (don't use as singleton)

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace imageEffect
{

////////////////////////////////////////////////////////////////////////////////
// descriptor

/// properties common on an effect and a descriptor
static property::OfxhPropSpec effectDescriptorStuff[] = {
    /* name                                 type                   dim. r/o default value */
    {kOfxPropType, property::ePropTypeString, 1, true, kOfxTypeImageEffect},
    {kOfxPropName, property::ePropTypeString, 1, false, "UNIQUE_NAME_NOT_SET"},
    {kOfxPropLabel, property::ePropTypeString, 1, false, ""},
    {kOfxPropShortLabel, property::ePropTypeString, 1, false, ""},
    {kOfxPropLongLabel, property::ePropTypeString, 1, false, ""},
    {kOfxPropPluginDescription, property::ePropTypeString, 1, false, "No plugin description."},
    {kOfxImageEffectPropSupportedContexts, property::ePropTypeString, 0, false, ""},
    {kOfxImageEffectPluginPropGrouping, property::ePropTypeString, 1, false, ""},
    {kOfxImageEffectPluginPropSingleInstance, property::ePropTypeInt, 1, false, "0"},
    {kOfxImageEffectPluginRenderThreadSafety, property::ePropTypeString, 1, false, kOfxImageEffectRenderInstanceSafe},
    {kOfxImageEffectPluginPropHostFrameThreading, property::ePropTypeInt, 1, false, "1"},
    {kOfxImageEffectPluginPropOverlayInteractV1, property::ePropTypePointer, 1, false, NULL},
    {kOfxImageEffectPropSupportsMultiResolution, property::ePropTypeInt, 1, false, "1"},
    {kOfxImageEffectPropSupportsTiles, property::ePropTypeInt, 1, false, "1"},
    {kOfxImageEffectPropTemporalClipAccess, property::ePropTypeInt, 1, false, "0"},
    {kOfxImageEffectPropSupportedPixelDepths, property::ePropTypeString, 0, false, ""},
    {kTuttleOfxImageEffectPropSupportedExtensions, property::ePropTypeString, 0, false, ""},
    {kTuttleOfxImageEffectPropEvaluation, property::ePropTypeDouble, 1, false, "-1"},
    {kOfxImageEffectPluginPropFieldRenderTwiceAlways, property::ePropTypeInt, 1, false, "1"},
    {kOfxImageEffectPropSupportsMultipleClipDepths, property::ePropTypeInt, 1, false, "0"},
    {kOfxImageEffectPropSupportsMultipleClipPARs, property::ePropTypeInt, 1, false, "0"},
    {kOfxImageEffectPropClipPreferencesSlaveParam, property::ePropTypeString, 0, false, ""},
    {kOfxImageEffectInstancePropSequentialRender, property::ePropTypeInt, 1, false, "0"},
    {kOfxPluginPropFilePath, property::ePropTypeString, 1, true, ""},
    {0}};

OfxhImageEffectNodeDescriptor::OfxhImageEffectNodeDescriptor()
    : OfxhImageEffectNodeBase(effectDescriptorStuff)
    , _plugin(NULL)
{
    /// @todo tuttle...
}

OfxhImageEffectNodeDescriptor::OfxhImageEffectNodeDescriptor(OfxhPlugin& plug)
    : OfxhImageEffectNodeBase(effectDescriptorStuff)
    , _plugin(&plug)
{
    _properties.setStringProperty(kOfxPluginPropFilePath, plug.getBinary().getBundlePath());
    tuttle::host::core().getHost().initDescriptor(*this);
}

OfxhImageEffectNodeDescriptor::OfxhImageEffectNodeDescriptor(const OfxhImageEffectNodeDescriptor& other, OfxhPlugin& plug)
    : OfxhImageEffectNodeBase(other._properties)
    , _plugin(&plug)
{
    _properties.setStringProperty(kOfxPluginPropFilePath, plug.getBinary().getBundlePath());
    tuttle::host::core().getHost().initDescriptor(*this);
}

OfxhImageEffectNodeDescriptor::OfxhImageEffectNodeDescriptor(const std::string& bundlePath, OfxhPlugin& plug)
    : OfxhImageEffectNodeBase(effectDescriptorStuff)
    , _plugin(&plug)
{
    _properties.setStringProperty(kOfxPluginPropFilePath, bundlePath);
    tuttle::host::core().getHost().initDescriptor(*this);
}

OfxhImageEffectNodeDescriptor::~OfxhImageEffectNodeDescriptor()
{
}

bool OfxhImageEffectNodeDescriptor::operator==(const This& other) const
{
    if(OfxhImageEffectNodeBase::operator!=(other) || attribute::OfxhParamSetDescriptor::operator!=(other) ||
       _clipsByOrder != other._clipsByOrder)
        return false;
    return true;
}

/**
 * @brief create a new clip and add this to the clip map
 */
attribute::OfxhClipImageDescriptor* OfxhImageEffectNodeDescriptor::defineClip(const std::string& name)
{
    attribute::OfxhClipImageDescriptor* c = new attribute::OfxhClipImageDescriptor(name);

    _clips[name] = c;
    _clipsByOrder.push_back(c);
    return c;
}

/// @warning tuttle some modifs here, doc needs update
/// get the interact description, this will also call describe on the interact
void OfxhImageEffectNodeDescriptor::initOverlayDescriptor(int bitDepthPerComponent, bool hasAlpha)
{
    if(_overlayDescriptor.getState() == interact::eUninitialised)
    {
        OfxPluginEntryPoint* overlayEntryPoint = getOverlayInteractMainEntry();
        // OK, we need to describe it, set the entry point and describe away
        _overlayDescriptor.setEntryPoint(overlayEntryPoint);
        _overlayDescriptor.describe(bitDepthPerComponent, hasAlpha);
    }
}

void OfxhImageEffectNodeDescriptor::addClip(const std::string& name, attribute::OfxhClipImageDescriptor* clip)
{
    _clips[name] = clip;
    _clipsByOrder.push_back(clip);
}
}
}
}
}
