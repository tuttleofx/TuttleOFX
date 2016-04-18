#ifndef _TUTTLE_HOST_OFX_ATTRIBUTE_CLIPIMAGEDESCRIPTOR_HPP_
#define _TUTTLE_HOST_OFX_ATTRIBUTE_CLIPIMAGEDESCRIPTOR_HPP_

#include "OfxhClipImageAccessor.hpp"
#include "OfxhClipDescriptor.hpp"

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/**
 * a clip image descriptor
 */
class OfxhClipImageDescriptor : virtual public OfxhClipImageAccessor, public OfxhClipDescriptor
{
public:
    typedef OfxhClipImageDescriptor This;

private:
    OfxhClipImageDescriptor();
    void init(const std::string& name);

public:
    /// constructor
    OfxhClipImageDescriptor(const std::string& name);

    virtual ~OfxhClipImageDescriptor() {}

    bool operator==(const This& other) const
    {
        if(OfxhClipDescriptor::operator!=(other))
            return false;
        return true;
    }

    bool operator!=(const This& other) const { return !This::operator==(other); }

    /** get a handle on the clip descriptor for the C api
     */
    OfxImageClipHandle getOfxImageClipHandle() const { return (OfxImageClipHandle) this; }

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(OfxhClipDescriptor);
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
struct is_virtual_base_of<tuttle::host::ofx::attribute::OfxhClipDescriptor,
                          tuttle::host::ofx::attribute::OfxhClipImageDescriptor> : public mpl::true_
{
};
}

#endif
