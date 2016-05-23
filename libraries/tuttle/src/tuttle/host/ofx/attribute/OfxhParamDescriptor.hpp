#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMDESCRIPTOR_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMDESCRIPTOR_HPP_

#include "OfxhParamAccessor.hpp"
#include "OfxhAttributeDescriptor.hpp"

#include <string>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/// is this a standard BaseType
bool isStandardType(const std::string& BaseType);

/// the Descriptor of a plugin parameter
class OfxhParamDescriptor : public OfxhAttributeDescriptor, virtual public OfxhParamAccessor
{
public:
    typedef OfxhParamDescriptor This;

private:
    OfxhParamDescriptor() {}

public:
    /// make a parameter, with the given type and name
    OfxhParamDescriptor(const std::string& type, const std::string& name);
    ~OfxhParamDescriptor() {}

    bool operator==(const This& other) const
    {
        if(OfxhAttributeDescriptor::operator!=(other))
            return false;
        return true;
    }

    bool operator!=(const This& other) const { return !This::operator==(other); }

    /// grab a handle on the parameter for passing to the C API
    OfxParamHandle getParamHandle() const { return (OfxParamHandle) this; }

    /// add standard param props, will call the below
    void initStandardParamProps(const std::string& type);

private:
    /// add standard properties to a params that can take an interact
    void initInteractParamProps(const std::string& type);

    /// add standard properties to a value holding param
    void initValueParamProps(const std::string& type, property::EPropType valueType, int dim);
    void initNoValueParamProps();

    /// add standard properties to a value holding param
    void initNumericParamProps(const std::string& type, property::EPropType valueType, int dim);

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
                          tuttle::host::ofx::attribute::OfxhParamDescriptor> : public mpl::true_
{
};
}

#endif
