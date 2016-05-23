#ifndef _TUTTLE_HOST_OFX_ATTRIBUTE_ATTRIBUTEDESCRIPTOR_HPP_
#define _TUTTLE_HOST_OFX_ATTRIBUTE_ATTRIBUTEDESCRIPTOR_HPP_

#include "OfxhAttributeAccessor.hpp"
#include <tuttle/host/ofx/property/OfxhSet.hpp>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

class OfxhAttributeDescriptor : virtual public OfxhAttributeAccessor
{
public:
    typedef OfxhAttributeDescriptor This;

private:
    OfxhAttributeDescriptor(const OfxhAttributeDescriptor& other);

public:
    OfxhAttributeDescriptor();
    OfxhAttributeDescriptor(const property::OfxhSet& properties);
    virtual ~OfxhAttributeDescriptor() = 0;

protected:
    property::OfxhSet _properties;

protected:
    void setProperties(const property::OfxhSet& properties)
    {
        _properties = properties;
        assert(getAttributeType().c_str());
    }

public:
    bool operator==(const This& other) const
    {
        if(_properties != other._properties)
            return false;
        return true;
    }

    bool operator!=(const This& other) const { return !This::operator==(other); }

    const property::OfxhSet& getProperties() const { return _properties; }

    property::OfxhSet& getEditableProperties() { return _properties; }

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
