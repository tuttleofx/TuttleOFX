#include "OfxhClipAccessor.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

OfxhClipAccessor::OfxhClipAccessor()
{
}

OfxhClipAccessor::~OfxhClipAccessor()
{
}

/**
 * is the clip optional
 */
bool OfxhClipAccessor::isOptional() const
{
    return getProperties().getIntProperty(kOfxImageClipPropOptional) != 0;
}

const bool OfxhClipAccessor::isConnected() const
{
    return getProperties().getIntProperty(kOfxImageClipPropConnected) != 0;
}

/**
 * @brief Connected
 * Says whether the clip is currently connected.
 */
void OfxhClipAccessor::setConnected(const bool isConnected)
{
    getEditableProperties().setIntProperty(kOfxImageClipPropConnected, 1);
}
}
}
}
}
