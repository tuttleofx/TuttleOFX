#ifndef _TUTTLE_HOST_OFX_ATTRIBUTE_CLIPDESCRIPTOR_HPP_
#define _TUTTLE_HOST_OFX_ATTRIBUTE_CLIPDESCRIPTOR_HPP_

#include "OfxhAttributeAccessor.hpp"
#include "OfxhAttributeDescriptor.hpp"
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
 * a clip descriptor
 */
class OfxhClipDescriptor : public OfxhAttributeDescriptor, virtual public OfxhClipAccessor
{
public:
    typedef OfxhClipDescriptor This;

public:
    /// constructor
    OfxhClipDescriptor();
    OfxhClipDescriptor(const property::OfxhSet&);

    virtual ~OfxhClipDescriptor() = 0;

    bool operator==(const This& other) const
    {
        if(OfxhAttributeDescriptor::operator!=(other))
            return false;
        return true;
    }

    bool operator!=(const This& other) const { return !This::operator==(other); }

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(OfxhAttributeDescriptor);
    }
};
}
}
}
}

// force boost::is_virtual_base_of value (used by boost::serialization)
namespace boost
{
template <>
struct is_virtual_base_of<tuttle::host::ofx::attribute::OfxhAttributeDescriptor,
                          tuttle::host::ofx::attribute::OfxhClipDescriptor> : public mpl::true_
{
};
}

#endif
