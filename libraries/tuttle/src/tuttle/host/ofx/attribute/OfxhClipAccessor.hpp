#ifndef _TUTTLE_HOST_OFX_ATTRIBUTE_CLIPACCESSOR_HPP_
#define _TUTTLE_HOST_OFX_ATTRIBUTE_CLIPACCESSOR_HPP_

#include "OfxhAttributeAccessor.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/**
 * Base to both descriptor and instance it
 * is used to basically fetch common properties
 * by function name
 */
class OfxhClipAccessor : virtual public OfxhAttributeAccessor
{
public:
    /// @brief base ctor, for a descriptor
    OfxhClipAccessor();

    virtual ~OfxhClipAccessor() = 0;

    /// is the clip optional
    bool isOptional() const;

    const bool isConnected() const;

    /**
     * @brief Connected
     * Says whether the clip is currently connected.
     */
    void setConnected(const bool isConnected = true);

    virtual bool verifyMagic() { return true; }
};
}
}
}
}

#endif
