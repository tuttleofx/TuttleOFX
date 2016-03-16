#include "OfxhClipImageDescriptor.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

OfxhClipImageDescriptor::OfxhClipImageDescriptor()
    : tuttle::host::ofx::attribute::OfxhClipDescriptor()
{
    init("");
}

/**
 * descriptor
 */
OfxhClipImageDescriptor::OfxhClipImageDescriptor(const std::string& name)
    : tuttle::host::ofx::attribute::OfxhClipDescriptor()
{
    init(name);
}

/**
 * descriptor
 */
void OfxhClipImageDescriptor::init(const std::string& name)
{
    /// properties common to the desciptor and instance
    /// the desc and set them, the instance cannot
    static const property::OfxhPropSpec clipImageDescriptorStuffs[] = {
        {kOfxClipPropType, property::ePropTypeString, 1, true, kOfxClipTypeImage},
        {kOfxImageEffectPropSupportedComponents, property::ePropTypeString, 0, false, ""},
        {kOfxImageEffectPropTemporalClipAccess, property::ePropTypeInt, 1, false, "0"},
        {kOfxImageClipPropIsMask, property::ePropTypeInt, 1, false, "0"},
        {kOfxImageClipPropFieldExtraction, property::ePropTypeString, 1, false, kOfxImageFieldDoubled},
        {kOfxImageEffectPropSupportsTiles, property::ePropTypeInt, 1, false, "1"},
        {0},
    };

    _properties.addProperties(clipImageDescriptorStuffs);
    _properties.setStringProperty(kOfxPropName, name);
}
}
}
}
}
