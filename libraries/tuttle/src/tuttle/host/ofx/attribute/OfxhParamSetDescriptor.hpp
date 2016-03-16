#ifndef _TUTTLE_HOST_OFX_PARAM_PARAMSETDESCRIPTOR_HPP_
#define _TUTTLE_HOST_OFX_PARAM_PARAMSETDESCRIPTOR_HPP_

#include "OfxhParamSetAccessor.hpp"
#include "OfxhParamDescriptor.hpp"

#include <boost/ptr_container/serialize_ptr_list.hpp>
#include <map>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

/// a set of parameters
class OfxhParamSetDescriptor : public OfxhParamSetAccessor
{
public:
    typedef OfxhParamSetDescriptor This;
    typedef std::map<std::string, OfxhParamDescriptor*> ParamDescriptorMap;
    typedef boost::ptr_list<OfxhParamDescriptor> ParamDescriptorList;
    ParamDescriptorMap _paramMap;
    ParamDescriptorList _paramList;

private:
    /// CC doesn't exist
    OfxhParamSetDescriptor(const This&);

public:
    /// default ctor
    OfxhParamSetDescriptor();

    /// dtor
    virtual ~OfxhParamSetDescriptor();

    bool operator==(const This& other) const
    {
        if(_paramList != other._paramList)
            return false;
        return true;
    }

    bool operator!=(const This& other) const { return !This::operator==(other); }

    /// obtain a handle on this set for passing to the C api
    OfxParamSetHandle getParamSetHandle() const { return (OfxParamSetHandle) this; }

    /// get the map of params
    const ParamDescriptorMap& getParams() const { return _paramMap; }
    ParamDescriptorMap& getParams() { return _paramMap; }

    /// get the list of params
    const ParamDescriptorList& getParamList() const { return _paramList; }

    /// define a param
    virtual OfxhParamDescriptor* paramDefine(const char* paramType, const char* name);

private:
    /// add a param in
    virtual void addParam(const std::string& name, OfxhParamDescriptor* p);

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& BOOST_SERIALIZATION_NVP(_paramList);
    }
};
}
}
}
}

#endif
