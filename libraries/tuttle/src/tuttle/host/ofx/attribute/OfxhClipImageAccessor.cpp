#include "OfxhClipImageAccessor.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/**
 * base ctor, for a descriptor
 */
OfxhClipImageAccessor::OfxhClipImageAccessor()
{
}

OfxhClipImageAccessor::~OfxhClipImageAccessor()
{
}

/** return a std::vector of supported comp
 */
const std::vector<std::string>& OfxhClipImageAccessor::getSupportedComponents() const
{
    return getProperties().fetchStringProperty(kOfxImageEffectPropSupportedComponents).getValues();
}

/** is the given component supported
 */
bool OfxhClipImageAccessor::isSupportedComponent(const std::string& comp) const
{
    return getProperties().findStringPropValueIndex(kOfxImageEffectPropSupportedComponents, comp) != -1;
}

/** does the clip do random temporal access
 */
bool OfxhClipImageAccessor::temporalAccess() const
{
    return getProperties().getIntProperty(kOfxImageEffectPropTemporalClipAccess) != 0;
}

/** is the clip a nominal 'mask' clip
 */
bool OfxhClipImageAccessor::isMask() const
{
    return getProperties().getIntProperty(kOfxImageClipPropIsMask) != 0;
}

/** how does this clip like fielded images to be presented to it
 */
const std::string& OfxhClipImageAccessor::getFieldExtraction() const
{
    return getProperties().getStringProperty(kOfxImageClipPropFieldExtraction);
}

/** is the clip a nominal 'mask' clip
 */
bool OfxhClipImageAccessor::supportsTiles() const
{
    return getProperties().getIntProperty(kOfxImageEffectPropSupportsTiles) != 0;
}
}
}
}
}
