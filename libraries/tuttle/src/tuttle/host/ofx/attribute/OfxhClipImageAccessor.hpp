#ifndef _TUTTLE_HOST_OFX_ATTRIBUTE_CLIPIMAGEACCESSOR_HPP_
#define _TUTTLE_HOST_OFX_ATTRIBUTE_CLIPIMAGEACCESSOR_HPP_

#include "OfxhClipAccessor.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/**
 *  Base to both descriptor and instance it
 * is used to basically fetch common properties
 * by function name
 */
class OfxhClipImageAccessor : virtual public attribute::OfxhClipAccessor
{
public:
    /// @brief base ctor, for a descriptor
    OfxhClipImageAccessor();

    virtual ~OfxhClipImageAccessor() = 0;

    /// @return a std::vector of supported comp
    const std::vector<std::string>& getSupportedComponents() const;

    /// @brief is the given component supported
    bool isSupportedComponent(const std::string& comp) const;

    /// @brief does the clip do random temporal access
    bool temporalAccess() const;

    /// @brief is the clip a nominal 'mask' clip
    bool isMask() const;

    /// @brief how does this clip like fielded images to be presented to it
    const std::string& getFieldExtraction() const;

    /// @brief is the clip a nominal 'mask' clip
    bool supportsTiles() const;

    /// get a handle on the clip descriptor/instance for the C api
    virtual OfxImageClipHandle getOfxImageClipHandle() const = 0;
};
}
}
}
}

#endif
